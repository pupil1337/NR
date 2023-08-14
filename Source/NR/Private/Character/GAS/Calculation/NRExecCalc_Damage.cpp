// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Calculation/NRExecCalc_Damage.h"

#include "Character/GAS/Attribute/NRAttributeSet.h"

struct FNRDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	FNRDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNRAttributeSet, Damage, Source, true);
	}
};

static const FNRDamageStatics& GetDamageStatics()
{
	static FNRDamageStatics DamageStatics;
	return DamageStatics;
}

UNRExecCalc_Damage::UNRExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(GetDamageStatics().DamageDef);
}

void UNRExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// TODO
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageStatics().DamageProperty, EGameplayModOp::Additive, 10));
}
