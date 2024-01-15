// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "HGridInventoryObject.generated.h"

class UHInventoryItemInstance;


//This will hold
UCLASS(BlueprintType)
class HEREWEGO_API UHGridInventoryObject : public UObject
{
	GENERATED_BODY()



public:
	UPROPERTY(Replicated)
	FHInventoryList InventoryList;

	UPROPERTY(NotReplicated)
	TObjectPtr<UHGridArray> GridArrayObject;

};
