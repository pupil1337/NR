// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Static/NRStatics.h"
#include "Character/NRCharacterMovementComponent.h"
#include "Character/Component/NRComponentBase.h"
#include "Character/GAS/NRAbilitySystemComponent.h"
#include "Character/GAS/NRAttributeSet.h"
#include "Character/GAS/NRGameplayAbility.h"
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

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("摄像机"));
	Camera->SetupAttachment(MeshArm, NAME_Socket_Camera);

	// NRAbilitySystemComponent
	NRAbilitySystemComponent = CreateDefaultSubobject<UNRAbilitySystemComponent>(TEXT("技能组件"));
	NRAbilitySystemComponent->SetIsReplicated(true);
	NRAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	NRAttributeSet = CreateDefaultSubobject<UNRAttributeSet>(TEXT("属性集"));
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

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		Spring->DestroyComponent();
		MeshArm->DestroyComponent();
		MeshLeg->DestroyComponent();
		Camera->DestroyComponent();
	}
}

void ANRCharacter::InitializeAttributes() const
{
	if (NRAbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = NRAbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		if (const FGameplayEffectSpecHandle SpecHandle = NRAbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.0f, EffectContext); SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = NRAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}	
}

void ANRCharacter::InitializeAbilities()
{
	if (HasAuthority() && NRAbilitySystemComponent)
	{
		for (TSubclassOf<UNRGameplayAbility>& Ability : DefaultAbilities)
		{
			if (Ability)
			{
				NRAbilitySystemComponent->GiveAbility(
					FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->GetAbilityInputID()), this)
				);
			}
		}
	}
}

void ANRCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NRAbilitySystemComponent)
	{
		NRAbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		InitializeAbilities();
	}
}

void ANRCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (NRAbilitySystemComponent)
	{
		NRAbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
	}
}

void ANRCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	SetMeshesVisibility();

	// Components本地控制端初始化
	for (UActorComponent* it: GetComponents())
	{
		if (UNRComponentBase* tNRComp = Cast<UNRComponentBase>(it))
		{
			tNRComp->PawnClientRestart();
		}
	}
}

void ANRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent)
	{
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
								EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &ThisClass::OnJumpInputPressed);
								EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ThisClass::OnJumpInputReleased);
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
}

void ANRCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		UpdateSpringLocation(DeltaSeconds);
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
	UNRStatics::SetFPS_SeparateFOV(MeshArm, true, true);
}

void ANRCharacter::UpdateSpringLocation(float DeltaSeconds) const
{
	Spring->SetRelativeLocation(FMath::VInterpTo(Spring->GetRelativeLocation(), FVector(0.0f, 0.0f, BaseEyeHeight) + SpringOffsetFPS, DeltaSeconds, 10));
}

// Inputs ===================================================================================
void ANRCharacter::SendLocalInputToASC(bool bPressed, ENRAbilityInputID InputID) const
{
	if (NRAbilitySystemComponent)
	{
		if (bPressed)
		{
			NRAbilitySystemComponent->AbilityLocalInputPressed(static_cast<uint32>(InputID));
		}
		else
		{
			NRAbilitySystemComponent->AbilityLocalInputReleased(static_cast<uint32>(InputID));
		}
	}
}

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

	// 限制角色俯仰角 上85° 下75°
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		if (const float NewRotationPitch = PlayerController->RotationInput.Pitch + PlayerController->GetControlRotation().Pitch;
			NewRotationPitch > 85.0f && NewRotationPitch < 360.0f - 75.0f)
		{
			PlayerController->RotationInput.Pitch = 0.0f;
		}
	}
}

void ANRCharacter::OnJumpInputPressed(const FInputActionValue& Value)
{
	SendLocalInputToASC(true, ENRAbilityInputID::EAIID_Jump);
}

void ANRCharacter::OnJumpInputReleased(const FInputActionValue& Value)
{
	SendLocalInputToASC(false, ENRAbilityInputID::EAIID_Jump);
}

void ANRCharacter::OnCrouchInput(const FInputActionValue& Value)
{
	if (NRAbilitySystemComponent)
	{
		if (NRAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Crouch"))))
		{
			SendLocalInputToASC(false, ENRAbilityInputID::EAIID_Crouch);
		}
		else
		{
			SendLocalInputToASC(true, ENRAbilityInputID::EAIID_Crouch);
		}
	}
}

void ANRCharacter::OnRunInput(const FInputActionValue& Value)
{
	//OnInputEvent_Run.Broadcast();
}

