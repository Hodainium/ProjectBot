// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "UObject/NoExportTypes.h"
#include "HGridArray.generated.h"

class UHGridEntry;
/**
 * 
 */
UCLASS()
class HEREWEGO_API UHGridArray : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	bool CanEntriesStack(UHGridEntry* BaseEntry, UHGridEntry* StackingEntry);

	//Does same as stack with entry except will also decrement the requesting entry with the amount of stacks added.
	//Returns amount of stacks added
	UFUNCTION()
	int32 TryToStackEntries(UHGridEntry* EntryToAddTo, UHGridEntry* RequestingEntry);

public:
	UHGridArray();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Functions")
	FHInventoryPoint GetInventorySize();

	UFUNCTION(BlueprintCallable, Category = "Inventory Functions")
	void SetInventorySize(FHInventoryPoint InSize);

	//Gets item pointer at specified point. Validates index before using. Returns nullptr if invalid index or no item found
	UFUNCTION()
	UHGridEntry* GetItemAtPoint(const FHInventoryPoint& InPoint);

	//Gets item pointer at specified index. Validates index before using. Returns nullptr if invalid index or no item found
	UFUNCTION()
	UHGridEntry* GetItemAtIndex(int32 Index) const;

	//Sets item at specified index. Validates index before using. Returns false if invalid index
	UFUNCTION()
	bool SetItemAtIndex(int32 Index, UHGridEntry* Entry);

	//Returns true if InventoryPoint is inbounds and valid otherwise return false
	UFUNCTION()
	bool IsPointInBoundsAndValid(const FHInventoryPoint& InPoint) const;

	//Validates that invPoint is in bounds and valid then returns index for current inventory size
	//If invalid returns INDEX_NONE
	UFUNCTION(BlueprintCallable)
	int32 InventoryPointToIndex(const FHInventoryPoint& InvPoint) const;

	//Converts index to inventoryPoint based on current inventory size
	//Checks if index is invalid or out of range for gridArray if so returns empty InvPoint
	UFUNCTION(BlueprintCallable)
	FHInventoryPoint IndexToInventoryPoint(int32 Index) const;

	//Get array of slotPoints that the item occupies at the specified location using it's currentDimensions. Validates input but does not validate output. Although all outputted indices should be valid
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentInventoryPointsAtPoint(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const;

	//Get array of int indices that the item occupies at the specified location using it's currentDimensions. Validates input but does not validate output. Although all outputted indices should be valid
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentItemIndicesAtPoint(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry, TArray<int32>& OutIndices) const;

	//Calls GetCurrentInventoryPointsAtPoint() with GridEntry's current location
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentInventoryPoints(UHGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const;

	//Calls GetCurrentItemIndicesAtPoint() with GridEntry's current location
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentItemIndices(UHGridEntry* GridEntry, TArray<int32>& OutIndices) const;

	UFUNCTION()
	bool GetIndicesForSizeAtPoint(const FHInventoryPoint& InPoint, const FHInventoryPoint& InSize, TArray<int32>& OutIndices) const;

	//Only returns true if no other items are blocking this point and data is valid
	//Returns false if a single item is blocking or data is invalid
	UFUNCTION(BlueprintCallable)
	bool FindNextBestSlotPoint(UHGridEntry* Entry, FHInventoryPoint& OutPoint) const;

	UFUNCTION()
	bool FindNextSlotPointForInstance(UHInventoryItemInstance* IncomingInstance, FHInventoryPoint& OutPoint) const;

	//Only returns true if no other items are blocking this point and data is valid
	//Returns false if a single item is blocking or data is invalid
	UFUNCTION(BlueprintCallable)
	bool IsFreeRoomAvailableAtPointForEntry(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry) const;

	UFUNCTION()
	bool IsFreeRoomAvailableAtPointWithSize(const FHInventoryPoint& InPoint, const FHInventoryPoint& InSize) const;

	//Gets an array of unique blocking entries at specified point using item's current dimensions. Does not include itself in the array
	//Returns false if input was invalid
	UFUNCTION(BlueprintCallable)
	bool GetAllBlockingItemsAtPoint(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry,
		TArray<UHGridEntry*>& OutBlockingItems) const;

	//Returns true if item dimensions don't extend past inventorygrid boundaries
	//Returns false if invalid data passed in or is past boundaries
	UFUNCTION()
	bool IsItemInBoundsAtPoint(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry) const;

	UFUNCTION()
	UHGridEntry* AddItemInstanceToGridAtPoint(const FHInventoryPoint& InPoint, UHInventoryItemInstance* ItemInstance, int32 StackCount);

	//Attempts to add to stack if instance matches.
	//Returns amount added
	//Returns 0 if error
	UFUNCTION()
	int32 TryToAddInstanceStackToEntry(UHGridEntry* EntryToAddTo, UHInventoryItemInstance* IncomingInstance, int32 StackCountToAdd);

	//Tries to find GridEntry with specified itemID. Returns false if it can't find it, true otherwise.
	//This function uses an algorithm that skips indexes based on item size
	UFUNCTION()
	UHGridEntry* FindEntryInGridByIDSmart(const int32 ItemID);

	//Gets struct item from StructList via id and returns its pointdata
	//Returns false if not found
	UFUNCTION()
	bool GetEntrySlotPointFromID(int32 ItemID, FHInventoryPoint& OutPoint);

private:
#pragma region Fast array handlers

	//Handles fast array events. Handles management between local items, entries, and the grid

	//Gets called from StructEntry's PreReplicatedRemove

	UFUNCTION()
	void HandlePreRemove(FHInventoryEntry& Entry);


	//Gets called from StructEntry's PostReplicatedAdd

	UFUNCTION()
	void HandlePostAdd(FHInventoryEntry& EntryToAdd);

	//Gets called from StructEntry's PostReplicatedChange

	UFUNCTION()
	void HandlePostChange(FHInventoryEntry& Entry);

	//Gets called from StructEntry's PostReplicatedReceive

	UFUNCTION()
	void UpdatePendingEntryPositionsGrid();

	//Removes specified item from grid using the entry's set dimensions and point
	//Skips indices that return a different item.
	//Returns false if item has invalid location, returns true otherwise
	bool RemoveItemFromGrid(UHGridEntry* EntryToRemove);

	//Adds specified item to grid using the entry's set dimensions and point
	//Skips indices that return a different item.
	//Returns false if item has invalid location, returns true otherwise
	bool AddItemToGrid(UHGridEntry* EntryToAdd);

	//////////////////////////////////////////////////////////////
	///
#pragma endregion Fast array handlers

	//This function refreshes the data of LocalGridEntry with it's linkedRepID
	//It returns true if item is found and refresh was succesful. Returns false otherwise.
	//Also returns if position was changed via update
	UFUNCTION()
	bool RefreshLocalEntry(UHGridEntry* LocalEntry, bool& bOutPositionChanged);

private:
	UPROPERTY()
	TArray<TObjectPtr<UHGridEntry>> GridArray;

	//Because we are working with fastarray we need to handle the order that the onrep functions are called.
	//We cache PendingItemsToMove in the postadd and postchange functions and finally add them in the last called function postRep
	UPROPERTY(Transient)
	TArray<TObjectPtr<UHGridEntry>> LocalPendingItemsToMove;

	//Current inventory size. Will be replicated
	UPROPERTY()
	FHInventoryPoint InventorySize;

	UPROPERTY()
	bool bIsDirty;

	UPROPERTY()
	bool bGridInitialized;
};
