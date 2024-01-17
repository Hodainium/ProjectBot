// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "HGridInventoryObject.generated.h"

class UHInventoryItemInstance;


//This will hold grid and fastarray
UCLASS(BlueprintType)
class HEREWEGO_API UHGridInventoryObject : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool TryAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool TryAddItemInstance(UHInventoryItemInstance* ItemInstance, int32 StackCount = 1);

public:
	UPROPERTY(Replicated)
	FHInventoryList InventoryList;

	UPROPERTY(NotReplicated)
	TObjectPtr<UHGridArray> GridArrayObject;
};
