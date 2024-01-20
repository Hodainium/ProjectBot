// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UHInventoryItemInstance;

USTRUCT(BlueprintType)
struct FHSlotsStruct
{
	GENERATED_BODY()

	UPROPERTY(Category = Slots)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots;

	UPROPERTY(Category = Slots)
	int32 NumSlots;

	UPROPERTY(Category = Slots)
	int32 ActiveSlotIndex;
};
