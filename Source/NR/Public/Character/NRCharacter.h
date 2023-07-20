// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GAS/NRAttributeSet.h"
#include "Types/NRGASTypes.h"
#include "NRCharacter.generated.h"

struct FInputActionValue;
class UNRGameplayAbility;
class UNRAttributeSet;
class UNRAbilitySystemComponent;
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
class NR_API ANRCharacter : public ANRCharacterBase, public IAbilitySystemInterface
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

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UNRAbilitySystemComponent> NRAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UNRAttributeSet> NRAttributeSet;

	// Settings
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|角色", DisplayName="摄像机-FPS弹簧臂相对eyes位置偏移")
	FVector SpringOffsetFPS = FVector(30.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|技能系统", DisplayName="角色默认Attribute-GameplayEffect")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|技能系统", DisplayName="角色默认技能列表")
	TArray<TSubclassOf<UNRGameplayAbility> > DefaultAbilities;
	
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

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return Cast<UAbilitySystemComponent>(NRAbilitySystemComponent); }

//~Begin This Class
	// Input Event Delegate
	FOnMoveInput   OnInputEvent_Move;
	FOnCrouchInput OnInputEvent_Crouch;
	FOnRunInput    OnInputEvent_Run;
	FOnJumped      OnJumpedEvent;

	// Getter
	FORCEINLINE USkeletalMeshComponent* GetMeshArm() const { return MeshArm; }
	FORCEINLINE FVector2D GetMoveInput() const { return MoveInputValue; } // 仅在控制端
	FORCEINLINE float GetHealth() const { return NRAttributeSet->GetHealth(); }
	FORCEINLINE float GetMaxHealth() const { return NRAttributeSet->GetMaxHealth(); }
	FORCEINLINE float GetShield() const { return NRAttributeSet->GetShield(); }
	FORCEINLINE float GetMaxShield() const { return NRAttributeSet->GetMaxShield(); }
	
private:
	void InitializeAttributes() const;
	void InitializeAbilities();
	
	// LocallyControlled
	void SetMeshesVisibility() const;
	void UpdateSpringLocation(float DeltaSeconds) const;
	
	// Inputs
	void SendLocalInputToASC(bool bPressed, ENRAbilityInputID InputID) const;
	FVector2D MoveInputValue = FVector2D::ZeroVector; // 仅在控制端
	void OnMoveInput(const FInputActionValue& Value);
	void OnLookInput(const FInputActionValue& Value);
	void OnJumpInputPressed(const FInputActionValue& Value);
	void OnJumpInputReleased(const FInputActionValue& Value);
	void OnCrouchInput(const FInputActionValue& Value);
	void OnRunInput(const FInputActionValue& Value);
};
