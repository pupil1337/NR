// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NRCharacterMovementComponent.generated.h"

/** NRCharacter的自定义移动模式 */
UENUM(BlueprintType)
enum ENRMovementMode
{
	NRMOVE_None UMETA(DisplayName="None"),
	NRMOVE_Ski  UMETA(DisplayName="Ski"),
	NRMOVE_MAX  UMETA(Hidden)
};

class NR_API FSavedMove_NR : public FSavedMove_Character
{
	using Super = FSavedMove_Character;
	
public:
	FSavedMove_NR();
	
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;

	uint8 bWantsToRun: 1;
	uint8 bWantsToSki: 1;
};

class NR_API FNetworkPredictionData_Client_NR : public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;

public:
	FNetworkPredictionData_Client_NR(const UCharacterMovementComponent& ClientMovement);
	virtual FSavedMovePtr AllocateNewMove() override;
};

class ANRCharacter;

/**
 * 
 */
UCLASS()
class NR_API UNRCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	friend FSavedMove_NR;
	
public:
	// Settings - Run
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="角色移动: 奔跑", DisplayName="最大奔跑速度")
	float Run_MaxWalkSpeed = 600.0f;

	// Settings - Ski
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="角色移动: 滑行", DisplayName="最小滑行速度")
	float Ski_MinSpeed = 200.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="角色移动: 滑行", DisplayName="开始滑行冲量")
	float Ski_EnterImpulse = 500.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="角色移动: 滑行", DisplayName="滑行向下的力")
	float Ski_GravityForce = 5000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="角色移动: 滑行", DisplayName="滑行摩擦力")
	float Ski_Friction = 1.3f;
	
	UNRCharacterMovementComponent();
	virtual void InitializeComponent() override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;


	virtual bool IsMovingOnGround() const override;

	virtual bool CanCrouchInCurrentState() const override;
	
protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
//~Begin This Class
public:
	bool IsNRMovementMode(ENRMovementMode InNRMovementMode) const;
	
	void Run(bool bRun);
	void Ski(bool bSki);
	
private:
	void EnterSki();
	void ExitSki();
	void PhySki(float deltaTime, int32 Iterations);
	bool GetSkiSurface(FHitResult& Hit) const;

	// Transient
	UPROPERTY(Transient)
	ANRCharacter* NRCharacterOwner;
	
	bool bWantsToRun;
	bool bWantsToSki;
};
