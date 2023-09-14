// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Static/NRStatics.h"
#include "Character/NRCharacterMovementComponent.h"
#include "Character/Component/NRInventoryComponent.h"
#include "Character/GAS/NRAbilitySystemComponent.h"
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

	// AbilitySystemComponent
	AbilitySystemComponent = CreateDefaultSubobject<UNRAbilitySystemComponent>(TEXT("技能组件"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AttributeSet
	AS_Character = CreateDefaultSubobject<UNRAS_Character>(TEXT("属性集"));

	// InventoryComponent
	InventoryComponent = CreateDefaultSubobject<UNRInventoryComponent>(TEXT("背包组件"));
	InventoryComponent->SetIsReplicated(true);
	InventoryComponent->bAutoActivate = true;
	InventoryComponent->bWantsInitializeComponent = true;
}

void ANRCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Only For Preview
	Spring->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight) + SpringOffsetFPS);
}

void ANRCharacter::PreInitializeComponents()
{
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
}

void ANRCharacter::InitializeAttributes() const
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		if (const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1.0f, EffectContext); SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}	
}

void ANRCharacter::InitializeAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (TSubclassOf<UNRGameplayAbility>& Ability : DefaultAbilities)
		{
			if (Ability)
			{
				AbilitySystemComponent->GiveAbility(
					FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->GetAbilityInputID()), this)
				);
			}
		}
	}
}

void ANRCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		InitializeAbilities();
	}
}

void ANRCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	SetMeshesVisibility();

	InventoryComponent->OnPawnClientRestart();
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
								EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &ThisClass::OnJumpInput);
								EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ThisClass::OnJumpInput);
							}
							if (IA_Crouch)							{
								EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &ThisClass::OnCrouchInput);
							}
							if (IA_Run)
							{
								EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Started, this, &ThisClass::OnRunInput);
							}
							if (IA_Fire)
							{
								EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ThisClass::OnFireInput);
								EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &ThisClass::OnFireInput);
							}
							if (IA_Interact)
							{
								EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &ThisClass::OnInteractInput);
								EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Completed, this, &ThisClass::OnInteractInput);
							}
						}
					}
				}
			}
		}

		// Components注册输入事件
		InventoryComponent->OnSetupPlayerInputComponent(PlayerInputComponent);
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

bool ANRCharacter::CanJumpInternal_Implementation() const
{
	return /* !bIsCrouched && */ JumpIsAllowedInternal();
}

void ANRCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	
	OnJumpedEvent.Broadcast();
}

void ANRCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (AbilitySystemComponent)
	{
		if (GetCharacterMovement()->IsFalling())
		{
			if (!AbilitySystemComponent->HasMatchingGameplayTag(NRGameplayTag::State_Falling))
			{
				AbilitySystemComponent->AddLooseGameplayTag(NRGameplayTag::State_Falling);	
			}
		}
		else
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(NRGameplayTag::State_Falling);
		}
	}
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
	if (AbilitySystemComponent)
	{
		if (bPressed)
		{
			AbilitySystemComponent->AbilityLocalInputPressed(static_cast<uint32>(InputID));
		}
		else
		{
			AbilitySystemComponent->AbilityLocalInputReleased(static_cast<uint32>(InputID));
		}
	}
}

void ANRCharacter::OnMoveInput(const FInputActionValue& Value)
{
	if (Controller)
	{
		MoveInputValue = Value.Get<FInputActionValue::Axis2D>();
		AddMovementInput(FRotationMatrix(FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Type::X), MoveInputValue.Y);
		AddMovementInput(FRotationMatrix(FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Type::Y), MoveInputValue.X);

		if (MoveInputValue.Y != 1.0f)
		{
			SendLocalInputToASC(false, ENRAbilityInputID::EAIID_Run);
		}
	}
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

void ANRCharacter::OnJumpInput(const FInputActionValue& Value)
{
	SendLocalInputToASC(static_cast<bool>(Value.Get<FInputActionValue::Axis1D>()), ENRAbilityInputID::EAIID_Jump);
}

void ANRCharacter::OnCrouchInput(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		// 滑铲
		if (bRunning)
		{
			SendLocalInputToASC(true, ENRAbilityInputID::EAIID_Ski);
		}
		else if (bSkiing)
		{
			SendLocalInputToASC(false, ENRAbilityInputID::EAIID_Ski);
		}
		// 蹲伏
		else if (bIsCrouched && !bSkiing)
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
	if (MoveInputValue.Y == 1.0f)
	{
		if (AbilitySystemComponent)
		{
			if (!AbilitySystemComponent->HasMatchingGameplayTag(NRGameplayTag::GA_Run))
			{
				SendLocalInputToASC(true, ENRAbilityInputID::EAIID_Run);
			}
		}	
	}
}

void ANRCharacter::OnFireInput(const FInputActionValue& Value)
{
	SendLocalInputToASC(static_cast<bool>(Value.Get<FInputActionValue::Axis1D>()), ENRAbilityInputID::EAIID_Fire);
}

void ANRCharacter::OnInteractInput(const FInputActionValue& Value)
{
	SendLocalInputToASC(static_cast<bool>(Value.Get<FInputActionValue::Axis1D>()), ENRAbilityInputID::EAIID_Interact);
}

