// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/NRInteractInterface.h"
#include "NRMonster.generated.h"

class UAbilitySystemComponent;
class UNRAS_Monster;
class UGameplayEffect;

UCLASS()
class NR_API ANRMonster : public ACharacter, public IAbilitySystemInterface, public INRInteractInterface
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNRAS_Monster> AS_Monster;

	// Settings
	UPROPERTY(EditDefaultsOnly, Category = "配置|技能系统", DisplayName = "怪物默认GE (设置Attributes)")
	TSubclassOf<UGameplayEffect> GE_Default;
	
public:
	ANRMonster();

	virtual void PossessedBy(AController* NewController) override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	// INRInteractInterface
	virtual ENRInteractionType GetInteractionType() override { return ENRInteractionType::EIT_Monster; }

//~Begin This Class
	void InitializeAttributes() const;
};
