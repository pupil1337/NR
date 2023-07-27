// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Types/NRGASTypes.h"
#include "NRGameplayAbility.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;

USTRUCT()
struct NR_API FNRAbilityMeshMontage
{
	GENERATED_BODY()

	FNRAbilityMeshMontage()
		: Mesh(nullptr), Montage(nullptr)
	{
	}

	FNRAbilityMeshMontage(USkeletalMeshComponent* InMesh, UAnimMontage* InMontage) 
		: Mesh(InMesh), Montage(InMontage)
	{
	}
	
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	UAnimMontage* Montage;
};

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class NR_API UNRGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UNRGameplayAbility();
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
//~Begin This Class
	FORCEINLINE ENRAbilityInputID GetAbilityInputID() const { return NRAbilityInputID; }

public:
	// ----------------------------------------------------------------------------------------------------------------
	//	Animation [override] 因为原先AvatarActor默认它只有一个Mesh, 我们项目Character有多个mesh用来播放动画
	// ----------------------------------------------------------------------------------------------------------------

	void SetCurrentAbilityMeshMontage(USkeletalMeshComponent* InMesh, UAnimMontage* InMontage);

	bool FindAbilityMeshMontage(const USkeletalMeshComponent* InMesh, OUT FNRAbilityMeshMontage& OutAbilityMeshMontage);
	
private:
	/** Active montage being played by this ability */
	UPROPERTY()
	TArray<FNRAbilityMeshMontage> CurrentAbilityMeshMontages;
	
	// Animation end --------------------------------------------------------------------------------------------------

protected:
	UPROPERTY(VisibleDefaultsOnly)
	ENRAbilityInputID NRAbilityInputID = ENRAbilityInputID::EAIID_None;

	UPROPERTY(VisibleDefaultsOnly, DisplayName="自动激活")
	bool bAutoActiveOnGive = false;
};
