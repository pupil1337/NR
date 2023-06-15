// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/NRItemFactory.h"

#include "NRGameSingleton.h"
#include "Static/NRStatics.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Engine/DataTable.h"
#include "Table/Weapon/NRWeaponInformation.h"

ANRWeaponBase* UNRItemFactory::SpawnWeapon(const UObject* ContextObject, ENRWeaponType WeaponType)
{
	if (UWorld* World = ContextObject ? ContextObject->GetWorld() : nullptr)
	{
		if (UNRGameSingleton* NRGameSingleton = Cast<UNRGameSingleton>(GEngine ? GEngine->GameSingleton : nullptr))
		{
			if (const FNRWeaponInformationRow* WeaponInfo = UNRStatics::GetWeaponInformationRow(WeaponType))
			{
				if (!WeaponInfo->WeaponClass.IsNull())
				{
					TSharedPtr<FStreamableHandle> StreamableHandle = NRGameSingleton->StreamableManager.RequestSyncLoad(WeaponInfo->WeaponClass.ToSoftObjectPath());

					FActorSpawnParameters Params;
					Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					ANRWeaponBase* Weapon = World->SpawnActor<ANRWeaponBase>(WeaponInfo->WeaponClass.Get());
					
					StreamableHandle->ReleaseHandle();
					StreamableHandle.Reset();

					return Weapon;
				}
			}
		}
	}

	return nullptr;
}

void UNRItemFactory::SpawnWeapon(UObject* ContextObject, const FVector& Location)
{
	if (UWorld* World = ContextObject ? ContextObject->GetWorld() : nullptr)
	{
		if (UNRGameSingleton* NRGameSingleton = Cast<UNRGameSingleton>(GEngine ? GEngine->GameSingleton : nullptr))
		{
			if (const UDataTable* WeaponInformation = NRGameSingleton->WeaponInformationDataTable)
			{
				if (const FNRWeaponInformationRow* WeaponInfo = WeaponInformation->FindRow<FNRWeaponInformationRow>(TEXT("AR_01"), "AR_01"))
				{
					if (!WeaponInfo->WeaponClass.IsNull())
					{
						TSharedPtr<FStreamableHandle> StreamableHandle = NRGameSingleton->StreamableManager.RequestSyncLoad(WeaponInfo->WeaponClass.ToSoftObjectPath());

						FActorSpawnParameters Params;
						Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						ANRWeaponBase* Weapon = World->SpawnActor<ANRWeaponBase>(WeaponInfo->WeaponClass.Get(), Location, FRotator::ZeroRotator);
						Weapon->SetWeaponState(ENRWeaponState::EWS_Pickup);
						
						StreamableHandle->ReleaseHandle();
						StreamableHandle.Reset();
					}
				}
			}
		}	
	}
}
