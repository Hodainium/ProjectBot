// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGO_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Static Functions")
	static int32 GetColumn(const int32 ArrayIndex, const int32 Rows);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Static Functions")
	static int32 GetRow(const int32 ArrayIndex, const int32 Rows);
	
};


