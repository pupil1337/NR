// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Calculation/NRExecCalc_Character.h"

#include "Types/NRGASTypes.h"
#include "Character/GAS/Attribute/NRAS_Character.h"

struct FNRDamageCharacterStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	FNRDamageCharacterStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNRAS_Character, Damage, Source, true);
	}
};

static const FNRDamageCharacterStatics& GetDamageStatics()
{
	static FNRDamageCharacterStatics DamageCharacterStatics;
	return DamageCharacterStatics;
}

UNRExecCalc_Character::UNRExecCalc_Character()
{
	RelevantAttributesToCapture.Add(GetDamageStatics().DamageDef);
}

void UNRExecCalc_Character::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	float Damage = 0.0f;
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageStatics().DamageDef, EvalParams, Damage);

	Damage += FMath::Max(Spec.GetSetByCallerMagnitude(NRGameplayTag::Data_Damage), 0.0f);

	// TODO 护甲等
	
	if (Damage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageStatics().DamageProperty, EGameplayModOp::Additive, 10));	
	}
}
