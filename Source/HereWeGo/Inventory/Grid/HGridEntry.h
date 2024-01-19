// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "UObject/NoExportTypes.h"
#include "HGridEntry.generated.h"

class UHInventoryItemInstance;
/**
 * 
 */
UCLASS(BlueprintType)
class HEREWEGO_API UHGridEntry : public UObject
{
	GENERATED_BODY()

public:
	UHGridEntry();

	void LoadEntryData(const FHInventoryEntry& Entry);

	//Updates data in object. Returns true if position changed
	void UpdateData(const FHInventoryEntry& Entry, bool& bOutPositionChanged);

	//Updates data in object. Returns true if position changed
	void InitializeData(UHInventoryItemInstance* ItemInstance, FHInventoryPoint Point, bool bIsRotated, int32 InStackCount);

	//Can stack check functions/////////////////////////

	UFUNCTION()
	bool CanStackWith(UHGridEntry* OtherEntry);

	bool CanStackWith(UHInventoryItemInstance* ItemInstance);

	//Stack count functions/////////////////////////

	//Adds stack to entry item. Is safe caps max and checks for neg.
	//Returns amount succesfully added
	int32 AddStackCountSafe(int32 CountToAdd);

	void DecrementStackCount(int32 CountToRemove);

	//Getters and setters/////////////////////////
	int32 GetLinkedID() const;
	void SetReplicatedID(int32 RepID);

	int32 GetStackCount();
	void SetStackCount(int32 NewCount);

	int32 GetMaxStackCount();

	FHInventoryPoint GetCurrentDimensions() const;

	void SetTopLeftTilePoint(FHInventoryPoint InPoint);
	FHInventoryPoint GetTopLeftTilePoint() const;

	bool GetCanItemBeStacked();

	UHInventoryItemInstance* GetItemInstance() const;

private:
	UPROPERTY()
	TObjectPtr<UHInventoryItemInstance> ItemInstance = nullptr;

	UPROPERTY()
	FHInventoryPoint TopLeftTilePoint;

	UPROPERTY()
	bool IsRotated = false;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY()
	int32 LinkedRepID = INDEX_NONE;

	//NOT IMPLEMENTED YET. NOT EVEN SURE IF WE'RE GOING OT USE YET
	UPROPERTY()
	bool bIsPredicted;
};
