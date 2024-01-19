// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HSlotSizeConfigDA.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HEREWEGO_API UHSlotSizeConfigDA : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SlotOffset = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int SlotSize = 80;
};
