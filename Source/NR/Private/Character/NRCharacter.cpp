// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

const FName NAME_Socket_Camera(TEXT("SOCKET_Camera"));
const FName NAME_Socket_CameraToRoot(TEXT("SOCKET_CameraToRoot"));
const FName NAME_Bone_Spine_01(TEXT("spine_01"));

ANRCharacter::ANRCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bUseControllerRotationYaw = true;
	
	// Spring
	Spring = CreateDefaultSubobject<USpringArmComponent>(TEXT("弹簧臂"));
	Spring->SetupAttachment(GetCapsuleComponent());
	Spring->bDoCollisionTest = false;
	Spring->bUsePawnControlRotation = true;
	Spring->TargetArmLength = 0.0f;

	// MeshArm
	MeshArm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("手臂"));
	MeshArm->SetupAttachment(Spring);
	MeshArm->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MeshArm->CanCharacterStepUpOn = ECB_No;
	MeshArm->CastShadow = false;
	MeshArm->bOnlyOwnerSee = true;

	// MeshLeg
	MeshLeg = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("腿"));
	MeshLeg->SetupAttachment(GetMesh());
	MeshLeg->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MeshLeg->CanCharacterStepUpOn = ECB_No;
	MeshLeg->CastShadow = false;
	MeshLeg->bOnlyOwnerSee = true;

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("摄像机"));
	Camera->SetupAttachment(MeshArm, NAME_Socket_Camera);
}

void ANRCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// MeshArm
	const FVector CameraLocation = MeshArm->GetSocketTransform(NAME_Socket_CameraToRoot, RTS_ParentBoneSpace).GetLocation();
	MeshArm->SetRelativeLocation(FVector(-CameraLocation.Y, -CameraLocation.X, -CameraLocation.Z));
	
	// MeshLeg
	MeshLeg->HideBoneByName(NAME_Bone_Spine_01, PBO_None);
}

void ANRCharacter::PostNetInit()
{
	Super::PostNetInit();

	if (!IsLocallyControlled())
	{
		GetMesh()->SetRenderInMainPass(true);
	}
}

void ANRCharacter::BeginPlay()
{
	Super::BeginPlay();
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
							EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
						}
					}
				}
			}		
		}
	}
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

