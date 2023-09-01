// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Calculation/NRMMC_ShieldRegen.h"

#include "Character/GAS/Attribute/NRAS_Character.h"

UNRMMC_ShieldRegen::UNRMMC_ShieldRegen()
{
	ShieldDef.AttributeToCapture = UNRAS_Character::GetShieldAttribute();
	ShieldDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ShieldDef.bSnapshot = false;
	
	MaxShieldDef.AttributeToCapture = UNRAS_Character::GetMaxShieldAttribute();
	MaxShieldDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxShieldDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(ShieldDef);
	RelevantAttributesToCapture.Add(MaxShieldDef);
}

float UNRMMC_ShieldRegen::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;
	Params.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	Params.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Shield = 0.0f;
	GetCapturedAttributeMagnitude(ShieldDef, Spec, Params, Shield);
	Shield = FMath::Max<float>(0.0f, Shield);
	
	float MaxShield = 0.0f;
	GetCapturedAttributeMagnitude(MaxShieldDef, Spec, Params, MaxShield);
	MaxShield = FMath::Max<float>(0.0f, MaxShield);
	
	return FMath::Min<float>(MaxShield * 0.01f, MaxShield - Shield);
}
