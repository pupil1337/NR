// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

namespace NRCollisionProfile
{
	const FName Projectile_ProfileName = FName(TEXT("Projectile"));
	const FName Interact_ProfileName = FName(TEXT("Interact"));
}

namespace NRCollisionChannel
{
	constexpr ECollisionChannel ECC_Ability = ECC_GameTraceChannel1;
	constexpr ECollisionChannel ECC_Projectile = ECC_GameTraceChannel2;
	constexpr ECollisionChannel ECC_Interact = ECC_GameTraceChannel3;
}
