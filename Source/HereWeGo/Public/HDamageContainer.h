// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HDamageContainer.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct HEREWEGO_API FHDamageContainer
{
	GENERATED_BODY()

	FHDamageContainer()
	{}

	UPROPERTY(EditDefaultsOnly, meta = (ForceInlineRow, Categories="Damage.Type"), Category = Equipment)
	TMap<FGameplayTag, float> DamageValues;
};

