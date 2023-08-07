// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Cue/NRGC_Fire.h"

#include "NiagaraSystem.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Table/Weapon/NRWeaponSetting.h"


void UNRGC_Fire::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	if (IsValid(MyTarget) && EventType == EGameplayCueEvent::Executed)
	{
		if (const ANRWeaponBase* Weapon = Cast<ANRWeaponBase>(Parameters.SourceObject.Get()))
		{
			if (const FNRWeaponSettingRow* WeaponSettingRow = Weapon->GetWeaponSettingRow())
			{
				if (WeaponSettingRow->BulletType == ENRBulletType::EBT_Instant)
				{
					// TODO 散弹枪
					Weapon->FireTracer(WeaponSettingRow->InstantTraceVFX.Get(), Parameters.EffectContext.GetHitResult()->ImpactPoint);
				}
			}
		}
	}
}
