// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/NRItemFactory.h"

#include "NRGameSingleton.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Engine/DataTable.h"
#include "Table/Weapon/NRWeaponInformation.h"

void UNRItemFactory::SpawnWeapon(UObject* ContextObject, const FVector& Location)
{
	if (UWorld* World = ContextObject ? ContextObject->GetWorld() : nullptr)
	{
		if (UNRGameSingleton* NRGameSingleton = Cast<UNRGameSingleton>(GEngine ? GEngine->GameSingleton : nullptr))
		{
			if (const UDataTable* WeaponInformation = NRGameSingleton->WeaponInformationDataTable)
			{
				if (const FNRWeaponInformationRow* WeaponInfo = WeaponInformation->FindRow<FNRWeaponInformationRow>(TEXT("AR_01"), "111"))
				{
					if (!WeaponInfo->WeaponClass.IsNull())
					{
						TSharedPtr<FStreamableHandle> StreamableHandle = NRGameSingleton->StreamableManager.RequestSyncLoad(WeaponInfo->WeaponClass.ToSoftObjectPath());

						World->SpawnActor<ANRWeaponBase>(WeaponInfo->WeaponClass.Get(), Location, FRotator::ZeroRotator);
						
						StreamableHandle->ReleaseHandle();
						StreamableHandle.Reset();
					}
				}
			}
		}	
	}
}
