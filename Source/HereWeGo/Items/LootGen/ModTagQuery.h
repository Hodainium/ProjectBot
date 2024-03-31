// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ModTagQuery.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct HEREWEGO_API FHModTagQuery
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGameplayTagContainer AggregatedTags;
};
