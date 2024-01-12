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
	FIntPoint CurrentPosition;

	// For MOVE operation:
	UPROPERTY(EditAnywhere)
	FIntPoint TargetPosition;

	// For ROTATE operation:
	UPROPERTY(EditAnywhere)
	bool bRotate;

	// Additional property for prediction:
	UPROPERTY()
	FHInventoryPredictionKey PredictionKey;  // Unique identifier for prediction tracking
};

//A UObject that wraps the InventoryEntry struct. Is instantiated locally
UCLASS(BlueprintType)
class UHLocalInventoryEntry : public UObject
{
	GENERATED_BODY()

public:
	UHLocalInventoryEntry();

	FIntPoint GetCurrentDimensions() const;

	void Initialize(const FHInventoryEntry& Entry);

	//Updates data in object. Returns true if position changed
	void UpdateData(const FHInventoryEntry& Entry, bool& bOutPositionChanged);

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UHInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FIntPoint TopLeftTileIndex;

	UPROPERTY(BlueprintReadWrite)
	FIntPoint PreviousTopLeftTileIndex;

	UPROPERTY(BlueprintReadWrite)
	bool IsRotated = false;

	UPROPERTY(BlueprintReadWrite)
	bool PreviousIsRotated = false;

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
	TArray<UHLocalInventoryEntry*> GetAllEntries();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Functions")
	FIntPoint GetInventorySize();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory Functions")
	void SetInventorySize(int32 Width, int32 Height);

	void RebuildLocalGrid();

private:

	//GRID FUNCTIONS
	
	void UpdateProjectionPositionGrid(UHLocalInventoryEntry* EntryToMove);
	//SHouldnt be used
	bool ResolveGridConflict(UHLocalInventoryEntry* ConflictingEntry);

	///////////////////
	void FindNextBestSpot();

#pragma region Fast array handlers

	//Handles fast array events. Handles management between local items, entries, and the grid

	//PreReplicatedRemove

	UFUNCTION()
	void RemoveLocalItem(FHInventoryEntry& Entry);
	void RemoveProjectionFromGrid(UHLocalInventoryEntry* EntryToRemove);

	//PostReplicatedAdd

	UFUNCTION()
	void AddLocalItem(FHInventoryEntry& EntryToAdd);

	//PostReplicatedChange

	UFUNCTION()
	void UpdateLocalItem(FHInventoryEntry& Entry);

	//PostReplicatedReceive

	UFUNCTION()
	void UpdatePendingEntryPositionsGrid();
	void AddProjectionToGrid(UHLocalInventoryEntry* EntryToAdd);
	//////////////////////////////////////////////////////////////
	///
#pragma endregion Fast array handlers

	UFUNCTION()
	void ForcePendingGridUpdate();

	UFUNCTION()
	int32 TileToIndex(const FIntPoint& TileIndex) const;

	UFUNCTION()
	bool GetItemIndices(const UHLocalInventoryEntry* LocalEntry, TArray<int32>& OutIndices) const;

	UFUNCTION()
	bool RefreshLocalEntry(UHLocalInventoryEntry* LocalEntry, bool& bOutPositionChanged);

	UFUNCTION()
	UHLocalInventoryEntry* FindItemByID(int32 ItemID);

	UFUNCTION()
	bool FindServerEntryByID(int32 ItemID, FHInventoryEntry& OutEntry);

private:

	UPROPERTY(Replicated)
	FHInventoryList MasterList;

	UPROPERTY()
	TArray<TObjectPtr<UHLocalInventoryEntry>> LocalInventoryGridArray;

	UPROPERTY()
	TArray<TObjectPtr<UHLocalInventoryEntry>> LocalInventoryGridAccelerationArray;

	UPROPERTY()
	TArray<TObjectPtr<UHLocalInventoryEntry>> LocalItemsToMove;

	UPROPERTY()
	int32 InventoryHeight;

	UPROPERTY()
	int32 InventoryWidth;

	UPROPERTY()
	bool bIsDirty;
};
