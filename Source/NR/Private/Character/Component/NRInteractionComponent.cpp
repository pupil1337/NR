// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRInteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NRGameSingleton.h"
#include "NRStatics.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRBagComponent.h"
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

void UNRInteractionComponent::InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent)
{
	Super::InitLocallyControlledInputEvent(PlayerInputComponent);

	if (NRCharacter)
	{
		if (const APlayerController* PlayerController = NRCharacter->GetController<APlayerController>())
		{
			if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					if (IMC_Interaction)
					{
						EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_Interaction, 0);
						if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
						{
							if (IA_E)
							{
								EnhancedInputComponent->BindAction(IA_E, ETriggerEvent::Started, this, &ThisClass::StartInteractInput);
							}
						}
					}
				}
			}
		}
	}
}

void UNRInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (NRCharacter && NRCharacter->IsLocallyControlled() )
	{
		if (const APlayerController* PlayerController = NRCharacter->GetController<APlayerController>())
		{
			FHitResult HitResult;
			if (UNRStatics::CrosshairTrace(PlayerController, UNRGameSingleton::Get()->CommonSetting.InteractionDistance, HitResult))
			{
				if (AActor* HitActor = HitResult.GetActor())
				{
					if (const INRInteractionInterface* Interaction = Cast<INRInteractionInterface>(HitActor))
					{
						switch (Interaction->GetInteractionType())
						{
							case ENRInteractionType::EIT_Weapon:
								LookWeapon(HitActor);
								break;
							default:
								UE_LOG(LogTemp, Log, TEXT("Unknow Interact Actor Type, Actor: %s"), *HitActor->GetName());
								break;
						}
					}
				}
			}
		}
	}
}

void UNRInteractionComponent::LookWeapon(AActor* HitActor)
{
	const AActor* OldActor = LookingActor;
	if (OldActor && OldActor != HitActor)
	{
		// TODO
	}

	LookingActor = HitActor;
	// TODO
}

void UNRInteractionComponent::StartInteractInput()
{
	if (LookingActor)
	{
		Server_Interact(LookingActor);
	}
}

void UNRInteractionComponent::Server_Interact_Implementation(AActor* Actor)
{
	if (const INRInteractionInterface* Interaction = Cast<INRInteractionInterface>(Actor))
	{
		switch (Interaction->GetInteractionType())
		{
		case ENRInteractionType::EIT_Weapon:
			if (ANRWeaponBase* Weapon = Cast<ANRWeaponBase>(Actor))
			{
				if (NRCharacter)
				{
					if (UNRBagComponent* BagComponent = Cast<UNRBagComponent>(NRCharacter->GetComponentByClass(UNRBagComponent::StaticClass())))
					{
						// TODO
						// BagComponent->GetItemInWorld(Weapon);
					}	
				}
			}
			break;
		default:
			UE_LOG(LogTemp, Log, TEXT("Unknow Interact Actor Type, Actor: %s"), *Actor->GetName());
			break;
		}
	}
}
