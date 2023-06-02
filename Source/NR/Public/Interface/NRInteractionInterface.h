// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/NRInteractionTypes.h"
#include "NRInteractionInterface.generated.h"

class ANRCharacter;

UINTERFACE()
class UNRInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NR_API INRInteractionInterface
{
	GENERATED_BODY()

public:
	virtual ENRInteractionType GetInteractionType() const = 0;
};
