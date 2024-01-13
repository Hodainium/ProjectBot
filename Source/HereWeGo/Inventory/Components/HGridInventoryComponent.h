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
	Move = 0,  // Operation for moving an item within the inventory grid
	Rotate = 1, // Operation for rotating an item (if applicable)
	Swap = 2 
};

USTRUCT(BlueprintType)
struct FInventoryOperation
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

	UFUNCTION()
	bool IsPointInBounds(const FHInventoryPoint& InPoint) const;

	UFUNCTION()
	bool IsItemInBoundsAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const;

	UFUNCTION(BlueprintCallable)
	bool IsRoomAvailableAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const;

	UFUNCTION(BlueprintCallable)
	bool GetBlockingItems(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry, TArray<UHLocalGridEntry*>& OutBlockingItems) const;

	//Returns itemref if there is single blocking item. Otherwise return null. Returns null if multiple items blocking
	UFUNCTION(BlueprintCallable)
	UHLocalGridEntry* IsSingleBlockingItemAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const;

	UFUNCTION()
	bool GetCurrentInventoryPoints(const UHLocalGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const;

	UFUNCTION()
	bool GetCurrentItemIndices(const UHLocalGridEntry* GridEntry, TArray<int32>& OutIndices) const;

	UFUNCTION()
	bool RefreshLocalEntry(UHLocalGridEntry* LocalEntry, bool& bOutPositionChanged);

	UFUNCTION()
	UHLocalGridEntry* FindItemByID(int32 ItemID);

	UFUNCTION()
	UHLocalGridEntry* GetItemAtPoint(const FHInventoryPoint& InPoint);

	UFUNCTION()
	UHLocalGridEntry* GetItemAtIndex(int32 Index);

	UFUNCTION()
	bool FindServerEntryByID(int32 ItemID, FHInventoryEntry& OutEntry);

private:

	UPROPERTY(Replicated)
	FHInventoryList MasterList;

	UPROPERTY()
	TArray<TObjectPtr<UHLocalGridEntry>> LocalGridArray;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UHLocalGridEntry>> LocalPendingItemsToMove;

	/*UPROPERTY()
	TArray<TObjectPtr<UHLocalGridEntry>> LocalInventoryGridAccelerationArray;*/

	/*UPROPERTY()
	int32 InventoryHeight;

	UPROPERTY()
	int32 InventoryWidth;*/

	UPROPERTY()
	FHInventoryPoint InventorySize;

	UPROPERTY()
	bool bIsDirty;

	UPROPERTY()
	bool bGridInitialized;
};
