// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EHAbilityInputID : uint8
{
	//0 none
	None		UMETA(DisplayName = "None"),
	Attack1		UMETA(DisplayName = "Attack1"),
	Attack2		UMETA(DisplayName = "Attack2"),
	Dash		UMETA(DisplayName = "Dash"),
	Ability1	UMETA(DisplayName = "Ability1"),
	Ability2	UMETA(DisplayName = "Ability2"),
	Ability3	UMETA(DisplayName = "Ability3"),
	Ability4	UMETA(DisplayName = "Ability4"),
};
