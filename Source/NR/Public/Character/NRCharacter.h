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
	UPROPERTY(Category="配置|角色", VisibleAnywhere)
	TObjectPtr<USpringArmComponent> Spring;

	UPROPERTY(Category="配置|角色", VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshArm;

	UPROPERTY(Category="配置|角色", VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshLeg;

	UPROPERTY(Category="配置|角色", VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY()
	TObjectPtr<UNRAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UNRInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UNRAS_Character> AS_Character;

	// Settings
	UPROPERTY(Category="配置|角色", EditAnywhere)
	FVector SpringOffsetFPS = FVector(30.0f, 0.0f, 0.0f);

	UPROPERTY(Category="配置|技能系统", EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(Category="配置|技能系统", EditDefaultsOnly)
	TArray<TSubclassOf<UNRGameplayAbility> > DefaultAbilities;
	
	// Inputs
	UPROPERTY(Category="配置|输入", EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> IMC_Character;
	UPROPERTY(Category="配置|输入", EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(Category="配置|输入", EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Look;
	UPROPERTY(Category="配置|输入", EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Jump;
	UPROPERTY(Category="配置|输入", EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Crouch;
	UPROPERTY(Category="配置|输入", EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Run;
	UPROPERTY(Category="配置|输入", EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Fire;
	UPROPERTY(Category="配置|输入", EditDefaultsOnly)
	TObjectPtr<UInputAction> IA_Interact;
	
public:
	explicit ANRCharacter(const FObjectInitializer& ObjectInitializer);
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
	void OnInteractInput(const FInputActionValue& Value);
};
