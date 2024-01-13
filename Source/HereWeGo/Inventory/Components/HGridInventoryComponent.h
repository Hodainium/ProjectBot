// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "Components/ActorComponent.h"
#include "HGridInventoryComponent.generated.h"


class UHInventoryItemInstance;

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
UCLASS(BlueprintType)
class UHLocalGridEntry : public UObject
{
	GENERATED_BODY()

public:
	UHLocalGridEntry();

	FHInventoryPoint GetCurrentDimensions() const;

	void Initialize(const FHInventoryEntry& Entry);

	//Updates data in object. Returns true if position changed
	void UpdateData(const FHInventoryEntry& Entry, bool& bOutPositionChanged);

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEREWEGO_API UHGridInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHGridInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	///////// For blueprint

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Functions")
	TArray<UHLocalGridEntry*> GetAllEntries();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Functions")
	FHInventoryPoint GetInventorySize();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory Functions")
	void SetInventorySize(int32 Width, int32 Height);

	void InitializedLocalGrid();

private:

	//GRID FUNCTIONS
	
	
	//SHouldnt be used
	bool ResolveGridConflict(UHLocalGridEntry* ConflictingEntry);

	///////////////////
	void FindNextBestSpot();

	void UpdateProjectionGrid(UHLocalGridEntry* EntryToMove);
	void RemoveProjectionFromGrid(UHLocalGridEntry* EntryToRemove);

#pragma region Fast array handlers

	//Handles fast array events. Handles management between local items, entries, and the grid

	//PreReplicatedRemove

	UFUNCTION()
	void HandlePreRemove(FHInventoryEntry& Entry);
	

	//PostReplicatedAdd

	UFUNCTION()
	void HandlePostAdd(FHInventoryEntry& EntryToAdd);

	//PostReplicatedChange

	UFUNCTION()
	void HandlePostChange(FHInventoryEntry& Entry);

	//PostReplicatedReceive

	UFUNCTION()
	void UpdatePendingEntryPositionsGrid();
	void AddProjectionToGrid(UHLocalGridEntry* EntryToAdd);
	//////////////////////////////////////////////////////////////
	///
#pragma endregion Fast array handlers

#pragma region Fast array functions

	UFUNCTION()
	bool GetEntryForItemID(int32 ItemID, FHInventoryEntry& OutEntry);

	UFUNCTION()
	bool GetSlotPointForItemID(int32 ItemID, FHInventoryPoint& OutPoint);

#pragma endregion Fast array functions

	UFUNCTION()
	void ForcePendingGridUpdate();

	UFUNCTION(BlueprintCallable)
	int32 InventoryPointToIndex(const FHInventoryPoint& InvPoint) const;

	UFUNCTION(BlueprintCallable)
	FHInventoryPoint IndexToInventoryPoint(int32 Index) const;

	//Returns true if InventoryPoint is inbounds and valid otherwise return false
	UFUNCTION()
	bool IsPointInBoundsAndValid(const FHInventoryPoint& InPoint) const;

	//Returns true if item dimensions don't extend past inventorygrid boundaries
	//Returns false if invalid data passed in or is past boundaries
	UFUNCTION()
	bool IsItemInBoundsAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const;

	//Only returns true if no other items are blocking this point and data is valid
	//Returns false if a single item is blocking or data is invalid
	UFUNCTION(BlueprintCallable)
	bool IsFreeRoomAvailableAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const;

	//Gets an array of unique blocking entries at specified point using item's current dimensions. Does not include itself in the array
	//Returns false if input was invalid
	UFUNCTION(BlueprintCallable)
	bool GetAllBlockingItemsAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry,
	                         TArray<UHLocalGridEntry*>& OutBlockingItems) const;

	//Returns itemref if there is single blocking item. Otherwise return null. Returns null if multiple items blocking
	//NOT SURE IF WE SHOULD USE
	/*UFUNCTION(BlueprintCallable)
	UHLocalGridEntry* IsSingleBlockingItemAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const;*/

	//Calls GetCurrentInventoryPointsAtPoint() with GridEntry's current location
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentInventoryPoints(const UHLocalGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const;

	//Get array of slotPoints that the item occupies at the specified location using it's currentDimensions. Validates input but does not validate output. Although all outputted indices should be valid
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentInventoryPointsAtPoint(const FHInventoryPoint& InPoint, const UHLocalGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const;

	//Calls GetCurrentItemIndicesAtPoint() with GridEntry's current location
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentItemIndices(const UHLocalGridEntry* GridEntry, TArray<int32>& OutIndices) const;

	//Get array of int indices that the item occupies at the specified location using it's currentDimensions. Validates input but does not validate output. Although all outputted indices should be valid
	//Returns false if invalid input provided
	UFUNCTION()
	bool GetCurrentItemIndicesAtPoint(const FHInventoryPoint& InPoint, const UHLocalGridEntry* GridEntry, TArray<int32>& OutIndices) const;

	//This function refreshes the data of LocalGridEntry with it's linkedRepID
	//It returns true if item is found and refresh was succesful. Returns false otherwise.
	//Also returns if position was changed via update
	UFUNCTION()
	bool RefreshLocalEntry(UHLocalGridEntry* LocalEntry, bool& bOutPositionChanged);

	//Tries to find GridEntry with specified itemID. Returns false if it can't find it, true otherwise.
	//This function uses an algorithm that skips indexes based on item size
	UFUNCTION()
	UHLocalGridEntry* FindEntryInGridByIDSmart(const int32 ItemID);

	//Tries to find FHInventoryEntry with specified itemID. Returns false if it can't find it, true otherwise
	UFUNCTION()
	bool FindStructEntryByID(int32 ItemID, FHInventoryEntry& OutEntry);
	//Gets item pointer at specified point. Validates index before using. Returns nullptr if invalid index or no item found
	UFUNCTION()
	UHLocalGridEntry* GetItemAtPoint(const FHInventoryPoint& InPoint);

	//Gets item pointer at specified index. Validates index before using. Returns nullptr if invalid index or no item found
	UFUNCTION()
	UHLocalGridEntry* GetItemAtIndex(int32 Index) const;

private:

	UPROPERTY(Replicated)
	FHInventoryList MasterList;

	UPROPERTY()
	TArray<TObjectPtr<UHLocalGridEntry>> LocalGridArray;

	//Because we are working with fastarray we need to handle the order that the onrep functions are called.
	//We cache PendingItemsToMove in the postadd and postchange functions and finally add them in the last called function postRep
	UPROPERTY(Transient)
	TArray<TObjectPtr<UHLocalGridEntry>> LocalPendingItemsToMove;

	//This will be overlaid on top of a constructed grid. The plan is to send these to server, server responds with same key and confirms it or not.
	//May return a correction rather than just outright denying. So user will be able to predict in case server sends an item in the occupying slot.
	//Server always overrides predicted items and sends to "Next best slot" if no next slot is available we drop item.
	//Will that be a another prediction or linked to awaiting prediction key?
	UPROPERTY(Transient)
	TArray<FHInventoryOperation> PredictedOperations;

	UPROPERTY()
	FHInventoryPoint InventorySize;

	UPROPERTY()
	bool bIsDirty;

	UPROPERTY()
	bool bGridInitialized;
};
