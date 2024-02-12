// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HStatBarEntry.generated.h"

USTRUCT(BlueprintType)
struct HEREWEGO_API FHStatBarEntry
{
	GENERATED_BODY()

	FHStatBarEntry()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories = "UI.StatBar"))
	FGameplayTag EntryTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> RowIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int StartTotalPips;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int StartDefaultPips;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int StartTempPips;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int StartBonusPips;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int StartNegativePips;
};

USTRUCT(BlueprintType)
struct HEREWEGO_API FHStatBarDefContainer
{
	GENERATED_BODY()

	FHStatBarDefContainer()
	{}

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (ForceInlineRow, Categories = "UI.StatBar"), Category = StatBar)
	TMap<FGameplayTag, FHStatBarEntry> StatBarEntries;
};
