// Fill out your copyright notice in the Description page of Project Settings.


#include "HGridInventoryComponent.h"
#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

FIntPoint UHLocalInventoryEntry::GetCurrentDimensions() const
{
	FIntPoint UnrotatedItemDimensions = Instance->GetItemDimensions();
	return (IsRotated ? FIntPoint(UnrotatedItemDimensions.Y, UnrotatedItemDimensions.X) : UnrotatedItemDimensions);
}

void UHLocalInventoryEntry::Initialize(const FHInventoryEntry& Entry)
{
	Instance = Entry.Instance;

	TopLeftTileIndex = Entry.TopLeftTileIndex;

	IsRotated = Entry.IsRotated;

	int32 StackCount = Entry.StackCount;

	int32 LinkedRepID = Entry.ReplicationID;
}

void UHLocalInventoryEntry::UpdateData(const FHInventoryEntry& Entry, bool& bOutPositionChanged)
{
	bOutPositionChanged = false;
	if(Entry.IsRotated != IsRotated || Entry.TopLeftTileIndex != TopLeftTileIndex)
	{
		bOutPositionChanged = true;
		PreviousTopLeftTileIndex = TopLeftTileIndex;
		PreviousIsRotated = IsRotated;
	}

	Instance = Entry.Instance;
	TopLeftTileIndex = Entry.TopLeftTileIndex;
	IsRotated = Entry.IsRotated;
	int32 StackCount = Entry.StackCount;
	int32 LinkedRepID = Entry.ReplicationID;
}

// Sets default values for this component's properties
UHGridInventoryComponent::UHGridInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHGridInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHGridInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHGridInventoryComponent::RebuildLocalGrid()
{
	for (UHLocalInventoryEntry* LocalEntry : LocalInventoryGridAccelerationArray)
	{
		TArray<int32> CurrentItemIndices;
		GetItemIndices(LocalEntry, CurrentItemIndices);

		for (int32 Index : CurrentItemIndices)
		{
			LocalInventoryGridArray[Index] = LocalEntry;
		}
	}
}

void UHGridInventoryComponent::ForceAddEntryToGrid(UHLocalInventoryEntry* EntryToAdd) //todo not done
{
	TArray<int32> ItemIndicesToAdd;
	GetItemIndices(EntryToAdd, ItemIndicesToAdd);
	for (int32 Index : ItemIndicesToAdd)
	{
		if (LocalInventoryGridArray[Index] != nullptr && LocalInventoryGridArray[Index] != EntryToAdd)
		{
			ResolveGridConflict(LocalInventoryGridArray[Index]);
		}

		LocalInventoryGridArray[Index] = EntryToAdd;
	}
}

void UHGridInventoryComponent::RemoveEntryFromGrid(UHLocalInventoryEntry* EntryToRemove)
{
	TArray<int32> ItemIndicesToRemove;
	GetItemIndices(EntryToRemove, ItemIndicesToRemove);
	for (int32 Index : ItemIndicesToRemove)
	{
		LocalInventoryGridArray[Index] = nullptr;
	}
}

void UHGridInventoryComponent::UpdateSingleEntryPositionGrid(UHLocalInventoryEntry* EntryToMove)
{
	RemoveEntryFromGrid(EntryToMove);
	ForceAddEntryToGrid(EntryToMove);
}

void UHGridInventoryComponent::UpdatePendingEntryPositionsGrid()
{
	for (UHLocalInventoryEntry* Entry : LocalItemsToMove)
	{
		RemoveEntryFromGrid(Entry);
	}

	for (UHLocalInventoryEntry* Entry : LocalItemsToMove)
	{
		ForceAddEntryToGrid(Entry);
	}

	LocalItemsToMove.Reset();
}

bool UHGridInventoryComponent::ResolveGridConflict(UHLocalInventoryEntry* ConflictingEntry)
{
	bool bPositionChanged;
	RefreshLocalEntry(ConflictingEntry, bPositionChanged);

	if(bPositionChanged)
	{
		UpdateSingleEntryPositionGrid(ConflictingEntry);
	}
	else
	{
		UE_LOGFMT(LogHGame, Error, "Could not resolve conflict. VERY BAD");
	}

	return false;
}

void UHGridInventoryComponent::RemoveLocalItem(FHInventoryEntry& Entry)
{
	UHLocalInventoryEntry* EntryToRemove = nullptr;

	EntryToRemove = FindItemByID(Entry.ReplicationID);

	if (EntryToRemove)
	{
		RemoveEntryFromGrid(EntryToRemove);
		LocalInventoryGridAccelerationArray.Remove(EntryToRemove);
	}
	else
	{
		UE_LOG(LogHGame, Error, TEXT("Failed to remove item in removelocalitems()"));
	}
}

void UHGridInventoryComponent::AddLocalItem(FHInventoryEntry& EntryToAdd)
{
	UHLocalInventoryEntry* NewLocalEntry = NewObject<UHLocalInventoryEntry>();
	NewLocalEntry->Initialize(EntryToAdd);
	LocalInventoryGridAccelerationArray.Add(NewLocalEntry);
	LocalItemsToMove.AddUnique(NewLocalEntry);
}

void UHGridInventoryComponent::UpdateLocalItem(FHInventoryEntry& Entry)
{
	UHLocalInventoryEntry* EntryToUpdate = nullptr;

	EntryToUpdate = FindItemByID(Entry.ReplicationID);

	if (EntryToUpdate)
	{
		bool bPositionChanged;
		EntryToUpdate->UpdateData(Entry, bPositionChanged);

		if(bPositionChanged)
		{
			LocalItemsToMove.AddUnique(EntryToUpdate);
		}
	}
	else
	{
		UE_LOG(LogHGame, Error, TEXT("Failed to update item in removelocalitems()"));
	}
}

int32 UHGridInventoryComponent::TileToIndex(const FIntPoint& TileIndex) const
{
	return TileIndex.X + TileIndex.Y * InventoryWidth;
}

bool UHGridInventoryComponent::GetItemIndices(const UHLocalInventoryEntry* LocalEntry, TArray<int32>& OutIndices) const
{
	if(LocalEntry)
	{
		const FIntPoint CurrentDimensions = LocalEntry->GetCurrentDimensions();
		FIntPoint CurrentTile = FIntPoint();
		for (int i = 0; i < CurrentDimensions.X; i++)
		{
			CurrentTile.X = LocalEntry->TopLeftTileIndex.X + i;

			for (int j = 0; j < CurrentDimensions.Y; j++)
			{
				CurrentTile.Y = LocalEntry->TopLeftTileIndex.Y + j;

				OutIndices.Add(TileToIndex(CurrentTile));
			}
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::RefreshLocalEntry(UHLocalInventoryEntry* LocalEntry, bool& bOutPositionChanged)
{
	FHInventoryEntry ServerEntry;

	if(FindServerEntryByID(LocalEntry->LinkedRepID, ServerEntry))
	{
		LocalEntry->UpdateData(ServerEntry, bOutPositionChanged);
		return true;
	}

	return false;
}

UHLocalInventoryEntry* UHGridInventoryComponent::FindItemByID(int32 ItemID)
{
	for(UHLocalInventoryEntry* Entry : LocalInventoryGridAccelerationArray)
	{
		if(Entry->LinkedRepID == ItemID)
		{
			return Entry;
		}
	}
	//UE_LOGFMT(LogHGame, Error, "dd");

	UE_LOG(LogHGame, Error, TEXT("Could not find item by ID in local grid"));

	return nullptr;
}

bool UHGridInventoryComponent::FindServerEntryByID(int32 ItemID, FHInventoryEntry& OutEntry)
{
	for(const FHInventoryEntry& Entry : MasterList)
	{
		if(Entry.ReplicationID == ItemID)
		{
			OutEntry = Entry;
			return true;
		}
	}

	return false;
}

