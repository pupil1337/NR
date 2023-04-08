// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRArmAnimInstance.h"

#include "Character/NRCharacter.h"

void FNRArmAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::Initialize(InAnimInstance);
}

void FNRArmAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		if (NRCharacter->IsLocallyControlled())
		{
			
		}
	}
}

void FNRArmAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}
