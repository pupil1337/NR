// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAS/NRExecCalc_Monster.h"

#include "AI/GAS/NRAS_Monster.h"
#include "Character/GAS/NRGameplayAbility.h"

struct FNRDamageMonsterStatics
{
	FNRDamageMonsterStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNRAS_Monster, Damage, Source, true)
	}

	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage)
};

static const FNRDamageMonsterStatics& GetDamageMonsterStatics()
{
	static FNRDamageMonsterStatics DamageMonsterStatics;
	return DamageMonsterStatics;
}

UNRExecCalc_Monster::UNRExecCalc_Monster()
{
	RelevantAttributesToCapture.Add(GetDamageMonsterStatics().DamageDef);
}

void UNRExecCalc_Monster::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	float Damage = FMath::Max(Spec.GetSetByCallerMagnitude(NRGameplayTag::Data_Damage), 0.0f);

	// TODO 护甲等伤害计算
	
	if (Damage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetDamageMonsterStatics().DamageProperty, EGameplayModOp::Type::Additive, Damage));
	}
}
