// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRInteractionComponent.h"

#include "NRGameSingleton.h"
#include "NRStatics.h"
#include "Character/NRCharacter.h"
#include "Interface/NRInteractionInterface.h"


UNRInteractionComponent::UNRInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UNRInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UNRInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (NRCharacter)
	{
		if (const APlayerController* PlayerController = NRCharacter->GetController<APlayerController>())
		{
			FHitResult HitResult;
			if (UNRStatics::CrosshairTrace(PlayerController, UNRGameSingleton::Get()->CommonSetting.InteractionDistance, HitResult))
			{
				if (INRInteractionInterface* HitActor = Cast<INRInteractionInterface>(HitResult.GetActor()))
				{
					UE_LOG(LogTemp, Log, TEXT("Can Interact Actor: %s"), *HitActor->GetName());
				}
			}
		}
	}
}

