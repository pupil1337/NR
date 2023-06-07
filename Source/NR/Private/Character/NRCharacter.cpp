// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NRStatics.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacterMovementComponent.h"
#include "Character/Component/NRBagComponent.h"
#include "Character/Component/NRComponentBase.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

const FName NAME_Socket_Camera(TEXT("SOCKET_Camera"));
const FName NAME_Bone_Spine_01(TEXT("spine_01"));

ANRCharacter::ANRCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UNRCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

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

void ANRCharacter::PreInitializeComponents()
{
	// Components
	if (GetLocalRole() == ROLE_Authority)
	{
		for (TSubclassOf<UNRComponentBase> tClass: NRComponentClasses)
		{
			UNRComponentBase* tComp = NewObject<UNRComponentBase>(this, tClass);
			tComp->RegisterComponent();
			// TODO 目前所有组件都是同步的, 非常没必要. (要分: 同步组件、仅服务器组件、仅本地控制组件、仅所有客户端组件)
			tComp->SetIsReplicated(true);
		}	
	}
	
	Super::PreInitializeComponents();
}

void ANRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ANRCharacter, bRunning, COND_SimulatedOnly)
	DOREPLIFETIME_CONDITION(ANRCharacter, bSkiing, COND_SimulatedOnly)
}

void ANRCharacter::BeginPlay()
{
	Super::BeginPlay();

	// if (GetLocalRole() == ROLE_SimulatedProxy)
	// {
	// 	Spring->DestroyComponent();
	// 	MeshArm->DestroyComponent();
	// 	MeshLeg->DestroyComponent();
	// 	Camera->DestroyComponent();
	// 	SeparateFOVCheckBox->DestroyComponent();
	// }
}

void ANRCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	SetMeshesVisibility();
}

void ANRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const APlayerController* PlayerController = GetController<APlayerController>())
	{
		if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
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
							EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &ThisClass::OnMoveInput);
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
							EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &ThisClass::OnCrouchInput);
						}
						if (IA_Run)
						{
							EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Started, this, &ThisClass::OnRunInput);
						}
					}
				}
			}		
		}
	}

	// Components注册输入事件
	for (UActorComponent* it: GetComponents())
	{
		if (UNRComponentBase* tNRComp = Cast<UNRComponentBase>(it))
		{
			tNRComp->InitLocallyControlledInputEvent(PlayerInputComponent);
		}
	}
}

void ANRCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		UpdateSpringLocation(DeltaSeconds);
		UpdateWhetherFixSeparate();
	}
}

void ANRCharacter::Jump()
{
	if (const UNRCharacterMovementComponent* NRCharacterMovementComponent = GetCharacterMovement<UNRCharacterMovementComponent>())
	{
		if (NRCharacterMovementComponent->IsCrouching())
		{
			UnCrouch();
		}
		else
		{
			Super::Jump();
		}
	}
}

void ANRCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	
	OnJumpedEvent.Broadcast();
}

// FuncType-LocallyControlled ===================================================================================
void ANRCharacter::SetMeshesVisibility() const
{
	GetMesh()->SetRenderInMainPass(false);
	MeshLeg->HideBoneByName(NAME_Bone_Spine_01, PBO_None);
}

void ANRCharacter::UpdateSpringLocation(float DeltaSeconds) const
{
	Spring->SetRelativeLocation(FMath::VInterpTo(Spring->GetRelativeLocation(), FVector(0.0f, 0.0f, BaseEyeHeight) + SpringOffsetFPS, DeltaSeconds, 10));
}

void ANRCharacter::UpdateWhetherFixSeparate() const
{
	bool bNeedFixSeparate = false;
	if (UNRCharacterMovementComponent* NRCharacterMovementComponent = GetCharacterMovement<UNRCharacterMovementComponent>())
	{
		bNeedFixSeparate = NRCharacterMovementComponent->IsCrouching();
	}

	if (!bNeedFixSeparate)
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		if (FHitResult Res; GetWorld()->SweepSingleByChannel(
			Res,
			SeparateFOVCheckBox->GetComponentLocation(),
			SeparateFOVCheckBox->GetComponentLocation() + SeparateFOVCheckBox->GetComponentRotation().Vector() * 10.0f,
			FQuat(FRotator(0.0f)),
			ECC_Camera,
			SeparateFOVCheckBox->GetCollisionShape(),
			QueryParams
			))
		{
			bNeedFixSeparate = true;
		}
	}

	UNRStatics::SetFPS_SeparateFOV(MeshArm, true, bNeedFixSeparate);
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
	OnInputEvent_Move.Broadcast(Value);
}

void ANRCharacter::OnLookInput(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.Get<FInputActionValue::Axis2D>().Y);
	AddControllerYawInput(Value.Get<FInputActionValue::Axis2D>().X);

	// 限制角色俯仰角 上下80°
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		if (const float NewRotationPitch = PlayerController->RotationInput.Pitch + PlayerController->GetControlRotation().Pitch;
			NewRotationPitch > 80.0f && NewRotationPitch < 360.0f - 80.0f)
		{
			PlayerController->RotationInput.Pitch = 0.0f;
		}
	}
}

void ANRCharacter::OnCrouchInput(const FInputActionValue& Value)
{
	OnInputEvent_Crouch.Broadcast();
}

void ANRCharacter::OnRunInput(const FInputActionValue& Value)
{
	OnInputEvent_Run.Broadcast();
}

