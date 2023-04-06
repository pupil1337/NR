// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "NRCharacter.generated.h"

class USpringArmComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class NR_API ANRCharacter : public ACharacter
{
	GENERATED_BODY()

	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpringArmComponent> Spring;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> MeshArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> MeshLeg;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> Camera;

	// Settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|角色", DisplayName="胶囊体-半高")
	float OriginCapsuleHalfHeight = 88.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|角色", DisplayName="胶囊体-半径")
	float OriginCapsuleRadius = 34.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|角色", DisplayName="胶囊体-蹲伏半高")
	float OriginCapsuleCrouchHalfHeight = 60.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|角色", DisplayName="摄像机-站立相对地面位置")
	FVector CameraToRootLocation_Standing = FVector(30.0f, 0.0f, 154.0f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|角色", DisplayName="摄像机-蹲伏相对地面位置")
	FVector CameraToRootLocation_Crouching = FVector(30.0f, 0.0f, 88.0f);

	// Inputs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputMappingContext> IMC_Character;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Look;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Jump;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Crouch;
	
public:
	ANRCharacter();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

//~ This Class Begin
private:
	// Initializes
	void SetMeshesVisibleAndComponentsDestroy();

	// FuncType-LocallyControlleds
	void UpdateSpringLocation(float DeltaSeconds);
	
	// Inputs
	void OnMoveInput(const FInputActionValue& Value);
	void OnLookInput(const FInputActionValue& Value);
	void OnCrouchInput();
	void OnUnCrouchInput();
	
	// Transients
	UPROPERTY(Transient)
	uint8 bSpringNeedMove;
	UPROPERTY(Transient)
	FVector SpringCurrLocation;
	UPROPERTY(Transient)
	FVector SpringTargetLocation;
};
