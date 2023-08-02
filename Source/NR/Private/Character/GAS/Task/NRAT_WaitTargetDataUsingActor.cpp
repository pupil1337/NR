// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Task/NRAT_WaitTargetDataUsingActor.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Character/GAS/TargetActor/NRTA_Trace.h"

UNRAT_WaitTargetDataUsingActor* UNRAT_WaitTargetDataUsingActor::WaitTargetDataUsingActor(UGameplayAbility* OwningAbility, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* TargetActor, bool bCreateKeyIfNotValidForMorePredicting/* =false */)
{
	UNRAT_WaitTargetDataUsingActor* MyObj = NewAbilityTask<UNRAT_WaitTargetDataUsingActor>(OwningAbility);		//Register for task list here, providing a given FName as a key
	MyObj->ConfirmationType = ConfirmationType;
	MyObj->TargetActor = TargetActor;
	MyObj->bCreateKeyIfNotValidForMorePredicting = bCreateKeyIfNotValidForMorePredicting;
	return MyObj;
}

void UNRAT_WaitTargetDataUsingActor::Activate()
{
	// Need to handle case where target actor was passed into task
	if (Ability && TargetActor && IsValid(this))
	{
		RegisterTargetDataCallbacks();
		
		InitializeTargetActor();
		FinalizeTargetActor();
	}
	else
	{
		EndTask();
	}
}

void UNRAT_WaitTargetDataUsingActor::InitializeTargetActor()
{
	check(TargetActor);
	check(Ability);

	TargetActor->PrimaryPC = Ability->GetCurrentActorInfo()->PlayerController.Get();

	// If we spawned the target actor, always register the callbacks for when the data is ready.
	TargetActor->TargetDataReadyDelegate.AddUObject(this, &UNRAT_WaitTargetDataUsingActor::OnTargetDataReadyCallback);
	TargetActor->CanceledDelegate.AddUObject(this, &UNRAT_WaitTargetDataUsingActor::OnTargetDataCancelledCallback);
}

void UNRAT_WaitTargetDataUsingActor::FinalizeTargetActor()
{
	check(TargetActor);
	check(Ability);

	TargetActor->StartTargeting(Ability);

	if (TargetActor->ShouldProduceTargetData())
	{
		// If instant confirm, then stop targeting immediately.
		// Note this is kind of bad: we should be able to just call a static func on the CDO to do this. 
		// But then we wouldn't get to set ExposeOnSpawnParameters.
		if (ConfirmationType == EGameplayTargetingConfirmation::Instant)
		{
			TargetActor->ConfirmTargeting();
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			// Bind to the Cancel/Confirm Delegates (called from local confirm or from repped confirm)
			TargetActor->BindToConfirmCancelInputs();
		}
	}
}

void UNRAT_WaitTargetDataUsingActor::RegisterTargetDataCallbacks()
{
	if (!ensure(IsValid(this)))
	{
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	check(Ability);

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = TargetActor->ShouldProduceTargetDataOnServer;

	// If not locally controlled (server for remote client), see if TargetData was already sent
	// else register callback for when it does get here.
	if (!bIsLocallyControlled)
	{
		// Register with the TargetData callbacks if we are expecting client to send them
		if (!bShouldProduceTargetDataOnServer)
		{
			FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
			FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

			//Since multifire is supported, we still need to hook up the callbacks
			ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey ).AddUObject(this, &UNRAT_WaitTargetDataUsingActor::OnTargetDataReplicatedCallback);
			ASC->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey ).AddUObject(this, &UNRAT_WaitTargetDataUsingActor::OnTargetDataReplicatedCancelledCallback);

			ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey );

			SetWaitingOnRemotePlayerData();
		}
	}
}

void UNRAT_WaitTargetDataUsingActor::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	FGameplayAbilityTargetDataHandle MutableData = Data;

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		ASC->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	/** 
	 *  Call into the TargetActor to sanitize/verify the data. If this returns false, we are rejecting
	 *	the replicated target data and will treat this as a cancel.
	 *	
	 *	This can also be used for bandwidth optimizations. OnReplicatedTargetDataReceived could do an actual
	 *	trace/check/whatever server side and use that data. So rather than having the client send that data
	 *	explicitly, the client is basically just sending a 'confirm' and the server is now going to do the work
	 *	in OnReplicatedTargetDataReceived.
	 */
	if (TargetActor && !TargetActor->OnReplicatedTargetDataReceived(MutableData))
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			Cancelled.Broadcast(MutableData);
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(MutableData);
		}
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UNRAT_WaitTargetDataUsingActor::OnTargetDataReplicatedCancelledCallback()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	EndTask();
}

void UNRAT_WaitTargetDataUsingActor::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC)
	{
		return;
	}

	FScopedPredictionWindow	ScopedPrediction(ASC, ShouldReplicateDataToServer() && (bCreateKeyIfNotValidForMorePredicting && !ASC->ScopedPredictionKey.IsValidForMorePrediction()));
	
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			FGameplayTag ApplicationTag; // Fixme: where would this be useful?
			ASC->CallServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), Data, ApplicationTag, ASC->ScopedPredictionKey);
		}
		else if (ConfirmationType == EGameplayTargetingConfirmation::UserConfirmed)
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericConfirm, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}

	if (ConfirmationType != EGameplayTargetingConfirmation::CustomMulti)
	{
		EndTask();
	}
}

void UNRAT_WaitTargetDataUsingActor::OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient())
	{
		if (!TargetActor->ShouldProduceTargetDataOnServer)
		{
			ASC->ServerSetReplicatedTargetDataCancelled(GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey );
		}
		else
		{
			// We aren't going to send the target data, but we will send a generic confirmed message.
			ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::GenericCancel, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
		}
	}
	Cancelled.Broadcast(Data);
	EndTask();
}

void UNRAT_WaitTargetDataUsingActor::ExternalConfirm(bool bEndTask)
{
	if (TargetActor)
	{
		if (TargetActor->ShouldProduceTargetData())
		{
			TargetActor->ConfirmTargetingAndContinue();
		}
	}
	Super::ExternalConfirm(bEndTask);
}

void UNRAT_WaitTargetDataUsingActor::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());
	}
	Super::ExternalCancel();
}

void UNRAT_WaitTargetDataUsingActor::OnDestroy(bool bInOwnerFinished)
{
	if (TargetActor)
	{
		if (ANRTA_Trace* TraceTargetActor = Cast<ANRTA_Trace>(TargetActor))
		{
			TraceTargetActor->StopTargeting();
		}
		// else if (ANRGATA_T* GATA = Cast<ANRGATA_T>(TargetActor)) // TODO 拓展
		// {
		// 	GATA->Stop***();
		// }
		else
		{
			// TargetActor doesn't have a StopTargeting function
			TargetActor->SetActorTickEnabled(false);

			// Clear added callbacks
			TargetActor->TargetDataReadyDelegate.RemoveAll(this);
			TargetActor->CanceledDelegate.RemoveAll(this);

			AbilitySystemComponent->GenericLocalConfirmCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::ConfirmTargeting);
			AbilitySystemComponent->GenericLocalCancelCallbacks.RemoveDynamic(TargetActor, &AGameplayAbilityTargetActor::CancelTargeting);
			TargetActor->GenericDelegateBoundASC = nullptr;
		}
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

bool UNRAT_WaitTargetDataUsingActor::ShouldReplicateDataToServer() const
{
	if (!Ability || !TargetActor)
	{
		return false;
	}

	// Send TargetData to the server IFF we are the client and this isn't a GameplayTargetActor that can produce data on the server	
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	if (!Info->IsNetAuthority() && !TargetActor->ShouldProduceTargetDataOnServer)
	{
		return true;
	}

	return false;
}
