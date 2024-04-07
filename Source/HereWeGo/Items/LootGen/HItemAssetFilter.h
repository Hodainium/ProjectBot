// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HItemAssetFilter.generated.h"

enum class EHItemQuality : uint8;
/**
 * 
 */
USTRUCT(BlueprintType)
struct HEREWEGO_API FHItemSearchQuery
{
	GENERATED_BODY()

	FHItemSearchQuery()
	{}

public:
	FGameplayTagContainer RequiredTags;

	FGameplayTagContainer AccumulatedTags;

	TSet<EHItemQuality> BlockedModQualities;

	TSet<FName> BlockedAssetNames;
};
