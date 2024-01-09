// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HInventoryInterface.generated.h"

class UHEquipmentComponent;
class UHInventoryComponent;
class UHItemSlotComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UHInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HEREWEGO_API IHInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual UHInventoryComponent* GetInventoryComponent() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual UHEquipmentComponent* GetEquipmentComponent() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual UHItemSlotComponent* GetItemSlotComponent() const = 0;
};
