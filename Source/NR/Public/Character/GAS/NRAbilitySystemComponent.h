// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NRAbilitySystemComponent.generated.h"

class UNRGameplayAbility;
class USkeletalMeshComponent;

/** Data about montages that were played locally (all montages in case of server. predictive montages in case of client). Never replicated directly. */
USTRUCT()
struct NR_API FGameplayAbilityLocalAnimMontageForMesh
{
	GENERATED_BODY()

	FGameplayAbilityLocalAnimMontageForMesh()
		: Mesh(nullptr), LocalAnimMontageInfo()
	{
	}

	explicit FGameplayAbilityLocalAnimMontageForMesh(USkeletalMeshComponent* InMesh)
		: Mesh(InMesh), LocalAnimMontageInfo()
	{
	}

	FGameplayAbilityLocalAnimMontageForMesh(USkeletalMeshComponent* InMesh, const FGameplayAbilityLocalAnimMontage& InLocalAnimMontageInfo)
		: Mesh(InMesh), LocalAnimMontageInfo(InLocalAnimMontageInfo)
	{
	}

	// Mesh
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	// FGameplayAbilityLocalAnimMontage
	UPROPERTY()
	FGameplayAbilityLocalAnimMontage LocalAnimMontageInfo;
};

/** Data about montages that is replicated to simulated clients */
USTRUCT()
struct NR_API FGameplayAbilityRepAnimMontageForMesh
{
	GENERATED_BODY()

	FGameplayAbilityRepAnimMontageForMesh()
		: Mesh(nullptr), RepAnimMontageInfo()
	{
	}

	explicit FGameplayAbilityRepAnimMontageForMesh(USkeletalMeshComponent* InMesh)
		: Mesh(InMesh), RepAnimMontageInfo()
	{
	}

	FGameplayAbilityRepAnimMontageForMesh(USkeletalMeshComponent* InMesh, const FGameplayAbilityRepAnimMontage& InRepAnimMontageInfo)
		: Mesh(InMesh), RepAnimMontageInfo(InRepAnimMontageInfo)
	{
	}
	
	// Mesh
	UPROPERTY()
	USkeletalMeshComponent* Mesh;

	// FGameplayAbilityRepAnimMontage
	UPROPERTY()
	FGameplayAbilityRepAnimMontage RepAnimMontageInfo;
};

UCLASS()
class NR_API UNRAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

//~Begin This Class
public:
	// ----------------------------------------------------------------------------------------------------------------
	//	AnimMontage Support [Override] 因为原先AvatarActor默认它只有一个Mesh, 我们项目Character有多个mesh用来播放动画
	//
	//	Note: 从Ability中触发PlayMontage(), 模拟客户端通过Replicates触发同步Montage. 如果服务器某个Mesh在很短时间内播放两个Montage,
	//	可能导致同步变量只同步后一个, 从而造成模拟客户端只播放后一个Montage,
	//	暂不考虑这个, 因为原始ASC也有这个问题, 并且Ability很少在很短时间内播放两个不同的Montage
	// ----------------------------------------------------------------------------------------------------------------

	/** Plays a montage and handles replication and prediction based on passed in ability/activation info */
	float PlayMontageForMesh(UNRGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, USkeletalMeshComponent* InMesh, UAnimMontage* NewMontage, float InPlayRate, FName StartSectionName = NAME_None, float StartTimeSeconds = 0.0f);

	UFUNCTION()
	void OnRep_ReplicatedAnimMontageForMeshes();
	
	/** Plays a montage without updating replication/prediction structures. Used by simulated proxies when replication tells them to play a montage. */
	float PlayMontageSimulatedForMesh(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None);

private:
	/** Called when a prediction key that played a montage is rejected */
	void OnPredictiveMontageRejectedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* PredictiveMontage);

	/** Copy LocalAnimMontageInfo into RepAnimMontageInfo */
	void AnimMontage_UpdateReplicatedDataForMesh();
	// TODO void AnimMontage_UpdateReplicatedDataForMesh()

	// 通过Mesh查找本地/同步动画, 没找到则创建
	OUT FGameplayAbilityLocalAnimMontageForMesh& GetLocalAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh);
	OUT FGameplayAbilityRepAnimMontageForMesh& GetRepAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh);
	
	/** Set if montage rep happens while we don't have the animinstance associated with us yet */
	UPROPERTY()
	bool bPendingMontageRepForMesh;

	/** Data structure for montages that were instigated locally (everything if server, predictive if client. replicated if simulated proxy) */
	UPROPERTY()
	TArray<FGameplayAbilityLocalAnimMontageForMesh> LocalAnimMontageInfoForMeshes;

	/** Data structure for replicating montage info to simulated clients */
	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedAnimMontageForMeshes)
	TArray<FGameplayAbilityRepAnimMontageForMesh> RepAnimMontageInfoForMeshes;
	
	// AnimMontage Support end ----------------------------------------------------------------------------------------
	
public:
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(const TSubclassOf<UGameplayAbility>& AbilityClass);
};
