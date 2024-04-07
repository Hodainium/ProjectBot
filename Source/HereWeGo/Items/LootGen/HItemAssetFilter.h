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
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer BlockedTags;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer AccumulatedTags;

	UPROPERTY(BlueprintReadWrite)
	TSet<EHItemQuality> BlockedModQualities;

	UPROPERTY(BlueprintReadWrite)
	TSet<FName> BlockedAssetNames;
};
