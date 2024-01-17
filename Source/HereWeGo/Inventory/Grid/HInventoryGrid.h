// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "HInventoryGrid.generated.h"

/**
 * 
 */

class UHGridInventoryObject;
class UHGridInventoryComponent;

USTRUCT()
struct FHInventoryPredictionKey
{
	GENERATED_BODY()

	FHInventoryPredictionKey()
	{
		KeyID = INDEX_NONE;
	}
	FHInventoryPredictionKey(int32 InID)
	{
		KeyID = InID;
	}

	UPROPERTY()
	int32 KeyID;

	static FHInventoryPredictionKey GenerateUniqueKey()
	{
		static int32 NextKeyID = 0;
		return FHInventoryPredictionKey(NextKeyID++);  // Atomically increment and return
	}
};

UENUM(BlueprintType)
enum class EHUInventoryOperationType : uint8
{
	Move = 0,
	Rotate = 1,
	Swap = 2,
	Drop = 3
};

USTRUCT(BlueprintType)
struct FHInventoryOperation
{
	GENERATED_BODY()

	UPROPERTY()
	EHUInventoryOperationType OperationType;

	UPROPERTY()
	int32 ItemIdSource;

	UPROPERTY()
	int32 ItemIdTarget;

	UPROPERTY()
	FHInventoryPoint CurrentPosition;

	// For MOVE operation:
	UPROPERTY(EditAnywhere)
	FHInventoryPoint TargetPosition;

	// For ROTATE operation:
	UPROPERTY(EditAnywhere)
	bool bRotate;

	// Additional property for prediction:
	UPROPERTY()
	FHInventoryPredictionKey PredictionKey;  // Unique identifier for prediction tracking
};

//A UObject that wraps the InventoryEntry struct. Is instantiated locally
//Needs to be initialized before inserting into Grid
UCLASS(BlueprintType)
class UHGridItem : public UObject
{
	GENERATED_BODY()

public:
	UHGridItem();

	UHGridItem(UHInventoryItemInstance* InInstance);

	FHInventoryPoint GetCurrentDimensions() const;

	void SetReplicatedID(int32 RepID);

	void LoadEntryData(const FHInventoryEntry& Entry);

	//Updates data in object. Returns true if position changed
	void UpdateData(const FHInventoryEntry& Entry, bool& bOutPositionChanged);

	//Updates data in object. Returns true if position changed
	void InitializeData(UHInventoryItemInstance* ItemInstance, FHInventoryPoint Point, bool bIsRotated, int32 InStackCount);

	int32 GetStackCount()
	{
		return StackCount;
	}

	void SetStackCount(int32 NewCount)
	{
		StackCount = NewCount;
		//Broadcast a change here
	}

	//Adds stack to entry item. Is safe caps max and checks for neg.
	//Returns amount succesfully added
	int32 AddStackCountSafe(int32 CountToAdd)
	{
		if(CountToAdd < 1)
		{
			return 0;
		}

		int32 OldCount = StackCount;
		int32 NewCount = StackCount + CountToAdd;
		int32 MaxStack = GetMaxStackCount();

		if (NewCount > MaxStack)
		{
			NewCount = MaxStack;
			CountToAdd = NewCount - OldCount;
		}

		SetStackCount(NewCount);
		return CountToAdd;
	}

	void DecrementStackCount(int32 CountToRemove)
	{
		int32 NewCount = StackCount - CountToRemove;

		if(NewCount < 0)
		{
			NewCount = 0;
		}

		SetStackCount(NewCount);
	}

	int32 GetMaxStackCount();

	UFUNCTION()
	bool GetCanItemBeStacked();

	UFUNCTION()
	bool CanStackWith(UHGridItem* OtherEntry);
	bool CanStackWith(UHInventoryItemInstance* ItemInstance);

	int32 TryToAddInstanceStack(UHInventoryItemInstance* IncomingInstance, int32 StackCountToAdd);

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UHInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FHInventoryPoint TopLeftTilePoint;

	UPROPERTY(BlueprintReadWrite)
	bool IsRotated = false;

	UPROPERTY(BlueprintReadWrite)
	int32 StackCount = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 LinkedRepID = INDEX_NONE;

	//NOT IMPLEMENTED YET. NOT EVEN SURE IF WE'RE GOING OT USE YET
	UPROPERTY(BlueprintReadOnly)
	bool bIsPredicted;
};

UCLASS(Blueprintable) // Add this for Blueprint support
class HEREWEGO_API UHGridArray : public UObject
{
	GENERATED_BODY()

	UHGridArray();

	friend UHGridInventoryComponent;
	friend UHGridInventoryObject;

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Functions")
	FHInventoryPoint GetInventorySize();

	//Only returns true if no other items are blocking this point and data is valid
	//Returns false if a single item is blocking or data is invalid
	UFUNCTION(BlueprintCallable)
	bool FindNextBestSlotPoint(UHGridItem* Entry, FHInventoryPoint& OutPoint) const;

	UFUNCTION()
	UHGridItem* AddItemInstanceToGridAtPoint(const FHInventoryPoint& InPoint, UHInventoryItemInstance* ItemInstance, int32 StackCount);

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
	bool RemoveItemFromGrid(UHGridItem* EntryToRemove);

	//Adds specified item to grid using the entry's set dimensions and point
	//Skips indices that return a different item.
	//Returns false if item has invalid location, returns true otherwise
	bool AddItemToGrid(UHGridItem* EntryToAdd);

	//////////////////////////////////////////////////////////////
	///
#pragma endregion Fast array handlers

	//Gets struct item from StructList via id and returns its pointdata
	//Returns false if not found
	UFUNCTION()
	bool GetEntrySlotPointFromID(int32 ItemID, FHInventoryPoint& OutPoint);

	//In case grid is dirty while trying to access. Is this even possible to happen?
	/*UFUNCTION()
	void ForcePendingGridUpdate();*/

	//Validates that invPoint is in bounds and valid then returns index for current inventory size
	//If invalid returns INDEX_NONE
	UFUNCTION(BlueprintCallable)
	int32 InventoryPointToIndex(const FHInventoryPoint& InvPoint) const;

	//Converts index to inventoryPoint based on current inventory size
	//Checks if index is invalid or out of range for gridArray if so returns empty InvPoint
	UFUNCTION(BlueprintCallable)
	FHInventoryPoint IndexToInventoryPoint(int32 Index) const;

	//Returns true if InventoryPoint is inbounds and valid otherwise return false
	UFUNCTION()
	bool IsPointInBoundsAndValid(const FHInventoryPoint& InPoint) const;

	//Returns true if item dimensions don't extend past inventorygrid boundaries
	//Returns false if invalid data passed in or is past boundaries
	UFUNCTION()
	bool IsItemInBoundsAtPoint(const FHInventoryPoint& InPoint, UHGridItem* GridEntry) const;

	

	//Only returns true if no other items are blocking this point and data is valid
	//Returns false if a single item is blocking or data is invalid
	UFUNCTION(BlueprintCallable)
	bool IsFreeRoomAvailableAtPointForEntry(const FHInventoryPoint& InPoint, UHGridItem* GridEntry) const;

	UFUNCTION()
	bool FindNextSlotPointForInstance(UHInventoryItemInstance* IncomingInstance, FHInventoryPoint& OutPoint) const;

	UFUNCTION()
	bool IsFreeRoomAvailableAtPointWithSize(const FHInventoryPoint& InPoint, const FHInventoryPoint& InSize) const;

	UFUNCTION()
	bool GetIndicesForSizeAtPoint(const FHInventoryPoint& InPoint, const FHInventoryPoint& InSize, TArray<int32>& OutIndices) const;

	//Gets an array of unique blocking entries at specified point using item's current dimensions. Does not include itself in the array
	//Returns false if input was invalid
	UFUNCTION(BlueprintCallable)
	bool GetAllBlockingItemsAtPoint(const FHInventoryPoint& InPoint, UHGridItem* GridEntry,
		TArray<UHGridItem*>& OutBlockingItems) const;

	//Calls GetCurrentInventoryPointsAtPoint() with GridEntry's current location
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentInventoryPoints(const UHGridItem* GridEntry, TArray<FHInventoryPoint>& OutPoints) const;

	//Get array of slotPoints that the item occupies at the specified location using it's currentDimensions. Validates input but does not validate output. Although all outputted indices should be valid
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentInventoryPointsAtPoint(const FHInventoryPoint& InPoint, const UHGridItem* GridEntry, TArray<FHInventoryPoint>& OutPoints) const;

	//Calls GetCurrentItemIndicesAtPoint() with GridEntry's current location
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentItemIndices(const UHGridItem* GridEntry, TArray<int32>& OutIndices) const;

	//Get array of int indices that the item occupies at the specified location using it's currentDimensions. Validates input but does not validate output. Although all outputted indices should be valid
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentItemIndicesAtPoint(const FHInventoryPoint& InPoint, const UHGridItem* GridEntry, TArray<int32>& OutIndices) const;

	//This function refreshes the data of LocalGridEntry with it's linkedRepID
	//It returns true if item is found and refresh was succesful. Returns false otherwise.
	//Also returns if position was changed via update
	UFUNCTION()
	bool RefreshLocalEntry(UHGridItem* LocalEntry, bool& bOutPositionChanged);

	//Tries to find GridEntry with specified itemID. Returns false if it can't find it, true otherwise.
	//This function uses an algorithm that skips indexes based on item size
	UFUNCTION()
	UHGridItem* FindEntryInGridByIDSmart(const int32 ItemID);

	//Tries to find FHInventoryEntry with specified itemID. Returns false if it can't find it, true otherwise
	UFUNCTION()
	bool FindStructEntryByID(int32 ItemID, FHInventoryEntry& OutEntry);

	//Gets item pointer at specified point. Validates index before using. Returns nullptr if invalid index or no item found
	UFUNCTION()
	UHGridItem* GetItemAtPoint(const FHInventoryPoint& InPoint);

	//Gets item pointer at specified index. Validates index before using. Returns nullptr if invalid index or no item found
	UFUNCTION()
	UHGridItem* GetItemAtIndex(int32 Index) const;

	//Sets item at specified index. Validates index before using. Returns false if invalid index
	UFUNCTION()
	bool SetItemAtIndex(int32 Index, UHGridItem* Entry);

	UFUNCTION()
	bool CanEntriesStack(UHGridItem* BaseEntry, UHGridItem* StackingEntry);

private:
	UPROPERTY()
	TArray<TObjectPtr<UHGridItem>> GridArray;

	//Because we are working with fastarray we need to handle the order that the onrep functions are called.
	//We cache PendingItemsToMove in the postadd and postchange functions and finally add them in the last called function postRep
	UPROPERTY(Transient)
	TArray<TObjectPtr<UHGridItem>> LocalPendingItemsToMove;

	//This will be overlaid on top of a constructed grid. The plan is to send these to server, server responds with same key and confirms it or not.
	//May return a correction rather than just outright denying. So user will be able to predict in case server sends an item in the occupying slot.
	//Server always overrides predicted items and sends to "Next best slot" if no next slot is available we drop item.
	//Will that be another prediction or linked to awaiting prediction key?
	UPROPERTY(Transient)
	TArray<FHInventoryOperation> PredictedOperations;

	//Current inventory size. Will be replicated
	UPROPERTY()
	FHInventoryPoint InventorySize;

	UPROPERTY()
	bool bIsDirty;

	UPROPERTY()
	bool bGridInitialized;
};
