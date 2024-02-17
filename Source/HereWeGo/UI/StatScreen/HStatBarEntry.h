// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HStatBarEntry.generated.h"

USTRUCT(BlueprintType)
struct HEREWEGO_API FHStatBarPipData
{
	GENERATED_BODY()

	FHStatBarPipData()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DefaultPips;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TempPips;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BonusPips;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NegativePips;
};

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
	FHStatBarPipData StartPipData;
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
