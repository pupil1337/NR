// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRCharacterBase.h"
#include "InputActionValue.h"
#include "NRCharacter.generated.h"

class UNRSaveGame;
class USpringArmComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ANRWeaponBase;
class UBoxComponent;
class UNRComponentBase;
class UNRCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRunInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveInput, const FInputActionValue&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumped);

UCLASS(Abstract, Blueprintable)
class NR_API ANRCharacter : public ANRCharacterBase
{
	GENERATED_BODY()

	// Components
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpringArmComponent> Spring;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> MeshArm;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> MeshLeg;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> Camera;

	// Settings
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|角色", DisplayName="摄像机-FPS弹簧臂相对eyes位置偏移")
	FVector SpringOffsetFPS = FVector(30.0f, 0.0f, 0.0f);

	// Inputs
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputMappingContext> IMC_Character;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Look;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Jump;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Crouch;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Run;

	// Components
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|角色", DisplayName="需要创建的组件")
	TArray<TSubclassOf<UNRComponentBase>> NRComponentClasses;
	
public:
	ANRCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PreInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Jump() override;
	virtual void OnJumped_Implementation() override;

//~ This Class Begin

	// Input Event Delegate
	FOnMoveInput   OnInputEvent_Move;
	FOnCrouchInput OnInputEvent_Crouch;
	FOnRunInput    OnInputEvent_Run;
	FOnJumped      OnJumpedEvent;

	// Getter
	FORCEINLINE const TArray<TSubclassOf<UNRComponentBase> >& GetAllNRComponentClasses() const { return NRComponentClasses; }
	FORCEINLINE USkeletalMeshComponent* GetMeshArm() const { return MeshArm; }

	// NRCharacterMovementComponent
	UPROPERTY(Transient, Replicated)
	bool bRunning;
	UPROPERTY(Transient, Replicated)
	bool bSkiing;
	
private:
	// LocallyControlled
	void SetMeshesVisibility() const;
	void UpdateSpringLocation(float DeltaSeconds) const;
	
	// Inputs
	void OnMoveInput(const FInputActionValue& Value);
	void OnLookInput(const FInputActionValue& Value);
	void OnCrouchInput(const FInputActionValue& Value);
	void OnRunInput(const FInputActionValue& Value);
};
