// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryEnums.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EHItemQuality : uint8
{
	Quality0,
	Quality1,
	Quality2,
	Quality3,
	Quality4,
	Quality5,
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EHItemQuality, EHItemQuality::Count);
