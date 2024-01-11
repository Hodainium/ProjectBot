// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "Components/ActorComponent.h"
#include "HGridInventoryComponent.generated.h"


class UHInventoryItemInstance;

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable)
	void InitializeSlots();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Functions")
	FIntPoint& GetInventorySize();

	//GRID FUNCTIONS
	void RebuildLocalGrid();
	void ForceAddEntryToGrid(UHLocalInventoryEntry* EntryToAdd);
	void RemoveEntryFromGrid(UHLocalInventoryEntry* EntryToRemove);
	void UpdateSingleEntryPositionGrid(UHLocalInventoryEntry* EntryToMove);
	void UpdatePendingEntryPositionsGrid();
	bool ResolveGridConflict(UHLocalInventoryEntry* ConflictingEntry);
	///////////////////

	UFUNCTION()
	void RemoveLocalItem(FHInventoryEntry& Entry);

	UFUNCTION()
	void AddLocalItem(FHInventoryEntry& EntryToAdd);

	UFUNCTION()
	void UpdateLocalItem(FHInventoryEntry& Entry);

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

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UHLocalInventoryEntry>> LocalInventoryGridArray;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UHLocalInventoryEntry>> LocalInventoryGridAccelerationArray;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UHLocalInventoryEntry>> LocalItemsToAdd;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UHLocalInventoryEntry>> LocalItemsToChange;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UHLocalInventoryEntry>> LocalItemsToMove;

	UPROPERTY(BlueprintReadOnly)
	int32 InventoryHeight;

	UPROPERTY(BlueprintReadOnly)
	int32 InventoryWidth;
};
