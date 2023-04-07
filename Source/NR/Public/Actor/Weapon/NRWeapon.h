// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NRWeapon.generated.h"

class USkeletalMeshComponent;

UCLASS()
class NR_API ANRWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	
public:
	ANRWeapon();

protected:
	virtual void BeginPlay() override;
};
