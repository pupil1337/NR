// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NRAbilitySystemComponent.generated.h"

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

public:
	UNRAbilitySystemComponent();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	virtual bool GetShouldTick() const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	
//~Begin This Class
public:
	// ----------------------------------------------------------------------------------------------------------------
	//	AnimMontage Support [Override]
	//	因为原先AvatarActor默认它只有一个Mesh, 我们项目Character有多个mesh用来播放动画
	//
	//	TODO:
	//	从Ability中触发PlayMontage(), 模拟客户端通过Replicates触发同步Montage. 如果服务器某个Mesh在很短时间内播放两个Montage,
	//	可能导致同步变量只同步后一个, 从而造成模拟客户端只播放后一个Montage.
	// ----------------------------------------------------------------------------------------------------------------

	/** Plays a montage and handles replication and prediction based on passed in ability/activation info */
	float PlayMontageForMesh(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, USkeletalMeshComponent* InMesh, UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName = NAME_None, float StartTimeSeconds = 0.0f);

	UFUNCTION()
	void OnRep_ReplicatedAnimMontageForMeshes();
	
	/** Plays a montage without updating replication/prediction structures. Used by simulated proxies when replication tells them to play a montage. */
	float PlayMontageSimulatedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName = NAME_None);

	/** Stops whatever montage is currently playing. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check) */
	void CurrentMontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime = -1.0f);

	/** Stops current montage if it's the one given as the Montage param */
	void StopMontageIfCurrentForMesh(USkeletalMeshComponent* InMesh, const UAnimMontage& Montage, float OverrideBlendOutTime = -1.0f);

	/** Clear the animating ability that is passed in, if it's still currently animating */
	void ClearAnimatingAbilityForAllMeshes(UGameplayAbility* Ability);

	/** Jumps current montage to given section. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check) */
	void CurrentMontageJumpToSectionNameForMesh(USkeletalMeshComponent* InMesh, FName SectionName);
	
	/** Sets current montages next section name. Expectation is caller should only be stopping it if they are the current animating ability (or have good reason not to check) */
	void CurrentMontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName);

	/** Sets current montage's play rate */
	void CurrentMontageSetPlayRateForMesh(USkeletalMeshComponent* InMesh, float InPlayRate);

	/** Returns true if the passed in ability is the current animating ability */
	bool IsAnimatingAbilityForAnyMesh(UGameplayAbility* Ability) const;

	/** Returns the current animating ability */
	UGameplayAbility* GetAnimatingAbilityForAnyMesh();

	/** Returns montages that is currently playing */
	TArray<UAnimMontage*> GetCurrentMontages() const;

	/** Returns montage that is currently playing */
	UAnimMontage* GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh);

	/** Get SectionID of currently playing AnimMontage */
	int32 GetCurrentMontageSectionIDForMesh(USkeletalMeshComponent* InMesh);

	/** Get SectionName of currently playing AnimMontage */
	FName GetCurrentMontageSectionNameForMesh(USkeletalMeshComponent* InMesh);

	/** Get length in time of current section */
	float GetCurrentMontageSectionLengthForMesh(USkeletalMeshComponent* InMesh);

	/** Returns amount of time left in current section */
	float GetCurrentMontageSectionTimeLeftForMesh(USkeletalMeshComponent* InMesh);
	
private:
	/** Called when a prediction key that played a montage is rejected */
	void OnPredictiveMontageRejectedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* PredictiveMontage);

	/** Copy LocalAnimMontageInfo into RepAnimMontageInfo */
	void AnimMontage_UpdateReplicatedDataForMesh(USkeletalMeshComponent* InMesh);
	void AnimMontage_UpdateReplicatedDataForMesh(OUT FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfoForMesh);

	// 通过Mesh查找本地/同步动画, 没找到则创建
	OUT FGameplayAbilityLocalAnimMontageForMesh& GetLocalAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh);
	OUT FGameplayAbilityRepAnimMontageForMesh& GetRepAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh);

	/** RPC function called from CurrentMontageJumpToSectionNameForMesh, replicates to other clients */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CurrentMontageJumpToSectionNameForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);
	void Server_CurrentMontageJumpToSectionNameForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);
	bool Server_CurrentMontageJumpToSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName);

	/** RPC function called from CurrentMontageSetNextSectionNameForMesh, replicates to other clients */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CurrentMontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);
	void Server_CurrentMontageSetNextSectionNameForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);
	bool Server_CurrentMontageSetNextSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName);

	/** RPC function called from CurrentMontageSetPlayRateForMesh, replicates to other clients */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CurrentMontageSetPlayRateForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);
	void Server_CurrentMontageSetPlayRateForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);
	bool Server_CurrentMontageSetPlayRateForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate);
	
	/** Set if montage rep happens while we don't have the AnimInstance associated with us yet */
	UPROPERTY()
	bool bPendingMontageRepForMesh;

	/** Data structure for montages that were instigated locally (everything if server, predictive if client. replicated if simulated proxy) */
	UPROPERTY()
	TArray<FGameplayAbilityLocalAnimMontageForMesh> LocalAnimMontageInfoForMeshes;

	/** Data structure for replicating montage info to simulated clients */
	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedAnimMontageForMeshes)
	TArray<FGameplayAbilityRepAnimMontageForMesh> RepAnimMontageInfoForMeshes;

	// ----------------------------------------------------------------------------------------------------------------
	//	Input handling/targeting [Override]
	// ----------------------------------------------------------------------------------------------------------------	
public:
	virtual void AbilityLocalInputPressed(int32 InputID) override;

	// ----------------------------------------------------------------------------------------------------------------
	// Custom
	// ----------------------------------------------------------------------------------------------------------------
public:
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(const TSubclassOf<UGameplayAbility>& AbilityClass);

	/**
	 * 重新设置激活的Effect持续时间
	 * @param Handle ActiveGameplayEffectHandle
	 * @return 指定Handle的ActiveGameplayEffect是否存在
	 */
	bool RestartActiveGameplayEffectDuration(const FActiveGameplayEffectHandle& Handle);
};
