// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Actor/Weapon/NRWeapon.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

const FName NAME_Socket_Camera(TEXT("SOCKET_Camera"));
const FName NAME_Bone_Spine_01(TEXT("spine_01"));
const FName NAME_Socket_Weapon(TEXT("SOCKET_Weapon"));
const FName NAME_Separate_FOV_Alpha(TEXT("Separate_FOV Alpha"));
const FName NAME_Separate_Alpha(TEXT("Separate Alpha"));

ANRCharacter::ANRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = true;
	
	// Spring
	Spring = CreateDefaultSubobject<USpringArmComponent>(TEXT("弹簧臂"));
	Spring->SetupAttachment(GetCapsuleComponent());

	// MeshArm
	MeshArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("手臂"));
	MeshArm->SetupAttachment(Spring);
	MeshArm->SetRelativeLocation(FVector(3.977629, -0.000008, -165.516068));

	// MeshLeg
	MeshLeg = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("腿"));
	MeshLeg->SetupAttachment(GetMesh());
	MeshLeg->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("摄像机"));
	Camera->SetupAttachment(MeshArm, NAME_Socket_Camera);

	// SeparateFOVCheckBox
	SeparateFOVCheckBox = CreateDefaultSubobject<UBoxComponent>(TEXT("穿模检测盒"));
	SeparateFOVCheckBox->SetupAttachment(MeshArm, NAME_Socket_Camera);
}

void ANRCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Only For Preview
	Spring->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight) + SpringOffsetFPS);
}

void ANRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ANRCharacter, EquippedWeapon, COND_None)
}

void ANRCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		Spring->DestroyComponent();
		MeshArm->DestroyComponent();
		MeshLeg->DestroyComponent();
		Camera->DestroyComponent();
		SeparateFOVCheckBox->DestroyComponent();
	}

	// TODO:换成背包组件
	if (HasAuthority())
	{
		if (WeaponClass)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.Owner = this;
			EquippedWeapon = GetWorld()->SpawnActor<ANRWeapon>(WeaponClass, Params);
			OnRep_EquippedWeapon(nullptr);
		}
	}
}

void ANRCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	SetMeshesVisibility();
}

void ANRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				if (IMC_Character)
				{
					EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_Character, 0);
					if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
					{
						if (IA_Move)
						{
							EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ThisClass::OnMoveInput);
						}
						if (IA_Look)
						{
							EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ThisClass::OnLookInput);
						}
						if (IA_Jump)
						{
							EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &ThisClass::Jump);
						}
						if (IA_Crouch)
						{
							EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &ThisClass::Crouch, false);
							EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &ThisClass::UnCrouch, false);
						}
					}
				}
			}		
		}
	}
}

void ANRCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		UpdateSpringLocation(DeltaSeconds);
		UpdateWhetherSeparateFOV();
	}
}

// FuncType-LocallyControlleds ===================================================================================
void ANRCharacter::SetMeshesVisibility()
{
	GetMesh()->SetRenderInMainPass(false);
	MeshLeg->HideBoneByName(NAME_Bone_Spine_01, PBO_None);
}

void ANRCharacter::UpdateSpringLocation(float DeltaSeconds)
{
	Spring->SetRelativeLocation(FMath::VInterpTo(Spring->GetRelativeLocation(), FVector(0.0f, 0.0f, BaseEyeHeight) + SpringOffsetFPS, DeltaSeconds, 10));
}

void ANRCharacter::UpdateWhetherSeparateFOV()
{
	FHitResult Res;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (GetWorld()->SweepSingleByChannel(
		Res,
		SeparateFOVCheckBox->GetComponentLocation(),
		SeparateFOVCheckBox->GetComponentLocation() + SeparateFOVCheckBox->GetComponentRotation().Vector() * 10.0f,
		FQuat(FRotator(0.0f)),
		ECC_Camera,
		SeparateFOVCheckBox->GetCollisionShape(),
		QueryParams
		))
	{
#ifdef WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::Printf(TEXT("穿模.并修复")));
#endif
		if (EquippedWeapon) EquippedWeapon->SetFPS_SeparateFOV(true, true);
		SetFPS_SeparateFOV(true, true);
	}
	else
	{
		if (EquippedWeapon) EquippedWeapon->SetFPS_SeparateFOV(true, false);
		SetFPS_SeparateFOV(true, false);
	}
}

void ANRCharacter::SetFPS_SeparateFOV(bool bEnable, bool bSeparate)
{
	float SeparateFOVAlpha = bEnable ? 1.0f : 0.0f;
	float SeparateAlpha = bSeparate ? 0.1f : 1.0f;
	MeshArm->SetScalarParameterValueOnMaterials(NAME_Separate_FOV_Alpha, SeparateFOVAlpha);
	MeshArm->SetScalarParameterValueOnMaterials(NAME_Separate_Alpha, SeparateAlpha);
	MeshArm->SetCastShadow(!bSeparate);
}

// Inputs ===================================================================================
void ANRCharacter::OnMoveInput(const FInputActionValue& Value)
{
	if (Controller)
	{
		const FVector2d Value2D = Value.Get<FInputActionValue::Axis2D>();
		AddMovementInput(FRotationMatrix(FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Type::X), Value2D.Y);
		AddMovementInput(FRotationMatrix(FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Type::Y), Value2D.X);
	}
}

void ANRCharacter::OnLookInput(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.Get<FInputActionValue::Axis2D>().Y);
	AddControllerYawInput(Value.Get<FInputActionValue::Axis2D>().X);

	// 限制角色俯仰角 上下80°
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		float NewRotationPitch = PlayerController->RotationInput.Pitch + PlayerController->GetControlRotation().Pitch;
		if (NewRotationPitch > 80.0f && NewRotationPitch < 360.0f - 80.0f)
		{
			PlayerController->RotationInput.Pitch = 0.0f;
		}
	}
}

// Temp
void ANRCharacter::OnRep_EquippedWeapon(ANRWeapon* OldEquippedWeapon)
{
	if (OldEquippedWeapon)
	{
		// ....
	}

	if (IsLocallyControlled())
	{
		EquippedWeapon->AttachToComponent(MeshArm, FAttachmentTransformRules::KeepRelativeTransform, NAME_Socket_Weapon);
	}
	else
	{
		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, NAME_Socket_Weapon);
	}
}

