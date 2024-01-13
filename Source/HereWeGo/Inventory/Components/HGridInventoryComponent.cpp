// Fill out your copyright notice in the Description page of Project Settings.


#include "HGridInventoryComponent.h"
#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

FHInventoryPoint UHLocalGridEntry::GetCurrentDimensions() const
{
	FHInventoryPoint UnrotatedItemDimensions = Instance->GetItemDimensions();
	return (IsRotated ? FHInventoryPoint(UnrotatedItemDimensions.Y, UnrotatedItemDimensions.X) : UnrotatedItemDimensions);
}

void UHLocalGridEntry::Initialize(const FHInventoryEntry& Entry)
{
	Instance = Entry.Instance;

	TopLeftTilePoint = Entry.TopLeftTilePoint;

	IsRotated = Entry.IsRotated;

	int32 StackCount = Entry.StackCount;

	int32 LinkedRepID = Entry.ReplicationID;
}

void UHLocalGridEntry::UpdateData(const FHInventoryEntry& Entry, bool& bOutPositionChanged)
{
	bOutPositionChanged = false;
	if(Entry.IsRotated != IsRotated || Entry.TopLeftTilePoint != TopLeftTilePoint)
	{
		bOutPositionChanged = true;
	}

	Instance = Entry.Instance;
	TopLeftTilePoint = Entry.TopLeftTilePoint;
	IsRotated = Entry.IsRotated;
	int32 StackCount = Entry.StackCount;
	int32 LinkedRepID = Entry.ReplicationID;
}

// Sets default values for this component's properties
UHGridInventoryComponent::UHGridInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bIsDirty = false;
	// ...
}


// Called when the game starts
void UHGridInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

TArray<UHLocalGridEntry*> UHGridInventoryComponent::GetAllEntries()
{
	return 
}

FHInventoryPoint UHGridInventoryComponent::GetInventorySize()
{
	return InventorySize;
}

void UHGridInventoryComponent::SetInventorySize(int32 Width, int32 Height)
{
	InventorySize.X = Width;
	InventorySize.Y = Height;
}

void UHGridInventoryComponent::InitializedLocalGrid()
{
	for (FHInventoryEntry& Entry : MasterList)
	{
		UHLocalGridEntry* NewLocalEntry = NewObject<UHLocalGridEntry>();
		NewLocalEntry->Initialize(Entry);

		TArray<int32> CurrentItemIndices;
		GetCurrentItemIndices(NewLocalEntry, CurrentItemIndices);

		for (int32 Index : CurrentItemIndices)
		{
			LocalGridArray[Index] = LocalEntry;
		}
	}

}

void UHGridInventoryComponent::AddProjectionToGrid(UHLocalGridEntry* EntryToAdd) //todo not done
{
	TArray<int32> ItemIndicesToAdd;
	GetCurrentItemIndices(EntryToAdd, ItemIndicesToAdd);
	for (int32 Index : ItemIndicesToAdd)
	{
		if (LocalGridArray[Index] != nullptr && LocalGridArray[Index] != EntryToAdd)
		{
			//ResolveGridConflict(LocalGridArray[Index]);
			UE_LOGFMT(LogHGame, Error, "There is a conflict in position {x} between {item1} and {item2}. Breaking out this shouldnt happen", Index, LocalGridArray[Index], EntryToAdd);
			break;
		}

		LocalGridArray[Index] = EntryToAdd;
	}
}

bool UHGridInventoryComponent::GetEntryForItemID(int32 ItemID, FHInventoryEntry& OutEntry)
{
	FHInventoryEntry Found = FHInventoryEntry();

	for (FHInventoryEntry& Entry : MasterList)
	{
		if (Entry.ReplicationID == ItemID)
		{
			OutEntry = Entry;
			return true;
		}
	}

	return false;
}

bool UHGridInventoryComponent::GetSlotPointForItemID(int32 ItemID, FHInventoryPoint& OutPoint)
{
	FHInventoryEntry Entry = FHInventoryEntry();
	if(GetEntryForItemID(ItemID, Entry))
	{
		OutPoint = Entry.TopLeftTilePoint;
		return false;
	}

	return true;
}

void UHGridInventoryComponent::ForcePendingGridUpdate()
{
	UE_LOGFMT(LogHGame, Warning, "Forcing early grid update");
	UpdatePendingEntryPositionsGrid();
}

void UHGridInventoryComponent::RemoveProjectionFromGrid(UHLocalGridEntry* EntryToRemove)
{
	bIsDirty = true;
	TArray<int32> ItemIndicesToRemove;
	GetCurrentItemIndices(EntryToRemove, ItemIndicesToRemove);
	for (int32 Index : ItemIndicesToRemove)
	{
		LocalGridArray[Index] = nullptr;
	}
}

void UHGridInventoryComponent::UpdateProjectionGrid(UHLocalGridEntry* EntryToMove)
{
	RemoveProjectionFromGrid(EntryToMove);
	AddProjectionToGrid(EntryToMove);
	
}

void UHGridInventoryComponent::UpdatePendingEntryPositionsGrid()
{
	for (UHLocalGridEntry* Entry : LocalPendingItemsToMove)
	{
		RemoveProjectionFromGrid(Entry);
	}

	for (UHLocalGridEntry* Entry : LocalPendingItemsToMove)
	{
		AddProjectionToGrid(Entry);
	}

	LocalPendingItemsToMove.Reset();
	bIsDirty = false;
}

bool UHGridInventoryComponent::ResolveGridConflict(UHLocalGridEntry* ConflictingEntry)
{
	bool bPositionChanged;
	RefreshLocalEntry(ConflictingEntry, bPositionChanged);

	if(bPositionChanged)
	{
		UpdateProjectionGrid(ConflictingEntry);
	}
	else
	{
		UE_LOGFMT(LogHGame, Error, "Could not resolve conflict. VERY BAD");
	}

	return false;
}

void UHGridInventoryComponent::HandlePreRemove(FHInventoryEntry& Entry)
{
	bIsDirty = true;

	UHLocalGridEntry* EntryToRemove = nullptr;

	EntryToRemove = FindItemByID(Entry.ReplicationID);

	if (EntryToRemove)
	{
		RemoveProjectionFromGrid(EntryToRemove);
		LocalInventoryGridAccelerationArray.Remove(EntryToRemove);
	}
	else
	{
		UE_LOG(LogHGame, Error, TEXT("Failed to remove item in removelocalitems()"));
	}
}

void UHGridInventoryComponent::HandlePostAdd(FHInventoryEntry& EntryToAdd)
{
	bIsDirty = true;

	UHLocalGridEntry* NewLocalEntry = NewObject<UHLocalGridEntry>();
	NewLocalEntry->Initialize(EntryToAdd);
	//LocalInventoryGridAccelerationArray.Add(NewLocalEntry);
	LocalPendingItemsToMove.AddUnique(NewLocalEntry);
}

void UHGridInventoryComponent::HandlePostChange(FHInventoryEntry& Entry)
{
	bIsDirty = true;

	UHLocalGridEntry* EntryToUpdate = nullptr;

	EntryToUpdate = FindItemByID(Entry.ReplicationID);

	if (EntryToUpdate)
	{
		bool bPositionChanged;
		EntryToUpdate->UpdateData(Entry, bPositionChanged);

		if(bPositionChanged)
		{
			LocalPendingItemsToMove.AddUnique(EntryToUpdate);
		}
	}
	else
	{
		UE_LOG(LogHGame, Error, TEXT("Failed to update item in removelocalitems()"));
	}
}

int32 UHGridInventoryComponent::InventoryPointToIndex(const FHInventoryPoint& InPoint) const
{
	if(InPoint.IsValid() && IsPointInBounds(InPoint))
	{
		return InPoint.X + InPoint.Y * InventorySize.X;
	}
	else
	{
		return INDEX_NONE;
	}
	
}

FHInventoryPoint UHGridInventoryComponent::IndexToInventoryPoint(int32 Index) const
{
	if(Index != INDEX_NONE)
	{
		return FHInventoryPoint{ Index % InventorySize.X, Index / InventorySize.X };
	}
	else
	{
		return FHInventoryPoint{};
	}
	
}

bool UHGridInventoryComponent::IsPointInBounds(const FHInventoryPoint& InPoint) const
{
	return (InPoint.X < InventorySize.X && InPoint.Y < InventorySize.Y);
}

bool UHGridInventoryComponent::IsItemInBoundsAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const
{
	const FHInventoryPoint ItemSize = GridEntry->GetCurrentDimensions();
	return (InPoint.X + ItemSize.X < InventorySize.X && InPoint.Y + ItemSize.Y < InventorySize.Y);
}

bool UHGridInventoryComponent::IsRoomAvailableAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const
{
	if(GridEntry && InPoint.IsValid()) 
	{
		const FHInventoryPoint CurrentDimensions = GridEntry->GetCurrentDimensions();
		FHInventoryPoint CurrentTile = FHInventoryPoint();

		for (int i = 0; i < CurrentDimensions.X; i++)
		{
			CurrentTile.X = InPoint.X + i;

			for (int j = 0; j < CurrentDimensions.Y; j++)
			{
				CurrentTile.Y = CurrentTile.Y + j;


			}
		}

		return true;
	}

	return false;
}

void UHGridInventoryComponent::GetBlockingItems(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry,
	TArray<UHLocalGridEntry*>& OutBlockingItems) const
{
	if (GridEntry && InPoint.IsValid())
	{
		const FHInventoryPoint CurrentDimensions = GridEntry->GetCurrentDimensions();
		FHInventoryPoint CurrentTile = FHInventoryPoint();

		for (int i = 0; i < CurrentDimensions.X; i++)
		{
			CurrentTile.X = InPoint.X + i;

			for (int j = 0; j < CurrentDimensions.Y; j++)
			{
				CurrentTile.Y = CurrentTile.Y + j;


			}
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::GetCurrentInventoryPoints(const UHLocalGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const
{
	if (GridEntry && GridEntry->TopLeftTilePoint.IsValid())
	{
		const FHInventoryPoint CurrentDimensions = GridEntry->GetCurrentDimensions();
		FHInventoryPoint CurrentPoint = FHInventoryPoint();
		for (int i = 0; i < CurrentDimensions.X; i++)
		{
			CurrentPoint.X = GridEntry->TopLeftTilePoint.X + i;

			for (int j = 0; j < CurrentDimensions.Y; j++)
			{
				CurrentPoint.Y = GridEntry->TopLeftTilePoint.Y + j * InventorySize.X;

				OutPoints.Add(CurrentPoint);
			}
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::GetCurrentItemIndices(const UHLocalGridEntry* GridEntry, TArray<int32>& OutIndices) const
{
	if(GridEntry && GridEntry->TopLeftTileIndex != INDEX_NONE)
	{
		const FIntPoint CurrentDimensions = GridEntry->GetCurrentDimensions();
		int32 CurrentIndex = 0;
		for (int i = 0; i < CurrentDimensions.X; i++)
		{
			CurrentIndex = GridEntry->TopLeftTileIndex + i;

			for (int j = 0; j < CurrentDimensions.Y; j++)
			{
				CurrentIndex = GridEntry->TopLeftTileIndex + j* InventorySize.X;

				OutIndices.Add(CurrentIndex);
			}
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::RefreshLocalEntry(UHLocalGridEntry* LocalEntry, bool& bOutPositionChanged)
{
	FHInventoryEntry ServerEntry;

	if(FindServerEntryByID(LocalEntry->LinkedRepID, ServerEntry))
	{
		LocalEntry->UpdateData(ServerEntry, bOutPositionChanged);
		return true;
	}

	return false;
}

UHLocalGridEntry* UHGridInventoryComponent::FindItemByID(int32 ItemID)
{
	for(FHInventoryEntry& Entry : MasterList)
	{
		if (Entry.ReplicationID == ItemID)
		{
			return Entry;
		}
	}

	/*for(UHLocalGridEntry* Entry : LocalInventoryGridAccelerationArray)
	{
		if(Entry->LinkedRepID == ItemID)
		{
			return Entry;
		}
	}*/
	//UE_LOGFMT(LogHGame, Error, "dd");

	UE_LOG(LogHGame, Error, TEXT("Could not find item by ID in local grid"));

	return nullptr;
}

UHLocalGridEntry* UHGridInventoryComponent::GetItemAtPoint(const FHInventoryPoint& InPoint)
{
	if(IsPointInBounds(InPoint))
	{
		return LocalGridArray[InventoryPointToIndex(InPoint)];
	}

	return nullptr;
}

UHLocalGridEntry* UHGridInventoryComponent::GetItemAtIndex(int32 Index)
{
	if (Index >= 0 && Index < LocalGridArray.Num())
	{
		return LocalGridArray[Index];
	}
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

