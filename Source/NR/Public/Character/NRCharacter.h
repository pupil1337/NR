// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GAS/Attribute/NRAS_Character.h"
#include "Types/NRGASTypes.h"
#include "NRCharacter.generated.h"

class UNRInventoryComponent;
class UNRGameplayAbility;
class UNRAbilitySystemComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrouchInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRunInput);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoveInput, const FInputActionValue&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumped);

UCLASS(Abstract, Blueprintable)
class NR_API ANRCharacter : public ANRCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

	// Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> Spring;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> MeshArm;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> MeshLeg;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNRAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNRInventoryComponent> InventoryComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNRAS_Character> AS_Character;

	// Settings
	UPROPERTY(EditDefaultsOnly, Category="配置|角色", DisplayName="摄像机-FPS弹簧臂相对eyes位置偏移")
	FVector SpringOffsetFPS = FVector(30.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, Category="配置|技能系统", DisplayName="角色默认Attribute-GameplayEffect")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(EditDefaultsOnly, Category="配置|技能系统", DisplayName="角色默认技能列表")
	TArray<TSubclassOf<UNRGameplayAbility> > DefaultAbilities;
	
	// Inputs
	UPROPERTY(EditDefaultsOnly, Category="配置|输入")
	TObjectPtr<UInputMappingContext> IMC_Character;
	UPROPERTY(EditDefaultsOnly, Category="配置|输入")
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, Category="配置|输入")
	TObjectPtr<UInputAction> IA_Look;
	UPROPERTY(EditDefaultsOnly, Category="配置|输入")
	TObjectPtr<UInputAction> IA_Jump;
	UPROPERTY(EditDefaultsOnly, Category="配置|输入")
	TObjectPtr<UInputAction> IA_Crouch;
	UPROPERTY(EditDefaultsOnly, Category="配置|输入")
	TObjectPtr<UInputAction> IA_Run;
	UPROPERTY(EditDefaultsOnly, Category="配置|输入")
	TObjectPtr<UInputAction> IA_Fire;
	
public:
	ANRCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PreInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return Cast<UAbilitySystemComponent>(AbilitySystemComponent); }

//~Begin This Class
	// Input Event Delegate
	FOnMoveInput   OnInputEvent_Move;
	FOnCrouchInput OnInputEvent_Crouch;
	FOnRunInput    OnInputEvent_Run;
	FOnJumped      OnJumpedEvent;

	// Custom Movement
	UPROPERTY(Replicated)
	bool bRunning;
	UPROPERTY(Replicated)
	bool bSkiing;
	
	// Getter
	FORCEINLINE USkeletalMeshComponent* GetMeshArm() const { return MeshArm; }
	FORCEINLINE UNRInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE FVector2D GetMoveInput() const { return MoveInputValue; } // 仅在控制端
	FORCEINLINE float GetHealth() const { return AS_Character->GetHealth(); }
	FORCEINLINE float GetMaxHealth() const { return AS_Character->GetMaxHealth(); }
	FORCEINLINE float GetShield() const { return AS_Character->GetShield(); }
	FORCEINLINE float GetMaxShield() const { return AS_Character->GetMaxShield(); }
	
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
	void OnJumpInput(const FInputActionValue& Value);
	void OnCrouchInput(const FInputActionValue& Value);
	void OnRunInput(const FInputActionValue& Value);
	void OnFireInput(const FInputActionValue& Value);
};
