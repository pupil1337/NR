// Fill out your copyright notice in the Description page of Project Settings.


#include "NRPointBase.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

ANRPointBase::ANRPointBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetCapsuleComponent()->InitCapsuleSize(40.0f, 92.0f);
	GetCapsuleComponent()->SetShouldUpdatePhysicsVolume(false);
	
#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(GetCapsuleComponent());
#endif
}

