// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NRCharacterBase.generated.h"

UCLASS(Abstract, NotBlueprintable)
class BASEMODULE_API ANRCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANRCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
};
