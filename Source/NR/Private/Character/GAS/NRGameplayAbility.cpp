// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/NRGameplayAbility.h"

#include "AbilitySystemComponent.h"

UNRGameplayAbility::UNRGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void UNRGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bAutoActiveOnGive)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

// --------------------------------------------------------------------------------------------------------------------
//	Animation
// --------------------------------------------------------------------------------------------------------------------
void UNRGameplayAbility::SetCurrentAbilityMeshMontage(USkeletalMeshComponent* InMesh, UAnimMontage* InMontage)
{
	FNRAbilityMeshMontage AbilityMeshMontage;
	if (FindAbilityMeshMontage(InMesh, AbilityMeshMontage))
	{
		AbilityMeshMontage.Montage = InMontage;
	}
	else
	{
		CurrentAbilityMeshMontages.Add(FNRAbilityMeshMontage(InMesh, InMontage));
	}
}

bool UNRGameplayAbility::FindAbilityMeshMontage(const USkeletalMeshComponent* InMesh, OUT FNRAbilityMeshMontage& OutAbilityMeshMontage)
{
	for (const FNRAbilityMeshMontage& AbilityMeshMontage: CurrentAbilityMeshMontages)
	{
		if (AbilityMeshMontage.Mesh == InMesh)
		{
			OutAbilityMeshMontage = AbilityMeshMontage;
			return true;
		}
	}
	
	return false;
}
