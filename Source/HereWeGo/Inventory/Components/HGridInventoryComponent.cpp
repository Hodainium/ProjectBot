// Fill out your copyright notice in the Description page of Project Settings.


#include "HGridInventoryComponent.h"
#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

UHLocalGridEntry::UHLocalGridEntry()
{
}

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

bool UHGridInventoryComponent::RemoveItemFromGrid(UHLocalGridEntry* EntryToRemove)
{
	bIsDirty = true;
	TArray<int32> ItemIndicesToRemove;

	if (GetCurrentItemIndices(EntryToRemove, ItemIndicesToRemove))
	{
		for (int32 Index : ItemIndicesToRemove)
		{
			UHLocalGridEntry* OccupyingEntry = GetItemAtIndex(Index);
			if (OccupyingEntry == nullptr || OccupyingEntry == EntryToRemove)
			{
				SetItemAtIndex(Index, nullptr);
			}
			else
			{
				UE_LOGFMT(LogHGame, Error, "While deleting an item from grid one of its indices had a different item at {index}", Index);
			}
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::AddItemToGrid(UHLocalGridEntry* EntryToAdd) //todo not done
{
	TArray<int32> ItemIndicesToAdd;

	if(GetCurrentItemIndices(EntryToAdd, ItemIndicesToAdd))
	{
		for (int32 Index : ItemIndicesToAdd)
		{
			UHLocalGridEntry* ObjectAtIndex = GetItemAtIndex(Index);

			if (ObjectAtIndex != nullptr && ObjectAtIndex != EntryToAdd)
			{
				UE_LOGFMT(LogHGame, Error, "There is a conflict in position {x} between {item1} and {item2}. Breaking out this shouldnt happen", Index, LocalGridArray[Index], EntryToAdd);
				break;
			}

			SetItemAtIndex(Index, EntryToAdd);
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::GetEntrySlotPointFromID(int32 ItemID, FHInventoryPoint& OutPoint)
{
	FHInventoryEntry Entry;

	if(FindStructEntryByID(ItemID, Entry))
	{
		OutPoint = Entry.TopLeftTilePoint;

		return true;
	}

	return false;
}

//void UHGridInventoryComponent::ForcePendingGridUpdate()
//{
//	UE_LOGFMT(LogHGame, Warning, "Forcing early grid update");
//	UpdatePendingEntryPositionsGrid();
//}

void UHGridInventoryComponent::UpdateProjectionGrid(UHLocalGridEntry* EntryToMove)
{
	RemoveItemFromGrid(EntryToMove);
	AddItemToGrid(EntryToMove);
	
}

void UHGridInventoryComponent::UpdatePendingEntryPositionsGrid()
{
	for (UHLocalGridEntry* Entry : LocalPendingItemsToMove)
	{
		RemoveItemFromGrid(Entry);
	}

	for (UHLocalGridEntry* Entry : LocalPendingItemsToMove)
	{
		AddItemToGrid(Entry);
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

	EntryToRemove = FindEntryInGridByIDSmart(Entry.ReplicationID);

	if (EntryToRemove)
	{
		RemoveItemFromGrid(EntryToRemove);
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
	LocalPendingItemsToMove.AddUnique(NewLocalEntry);
}

void UHGridInventoryComponent::HandlePostChange(FHInventoryEntry& Entry)
{
	bIsDirty = true;

	UHLocalGridEntry* EntryToUpdate = nullptr;

	EntryToUpdate = FindEntryInGridByIDSmart(Entry.ReplicationID);

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
	if(IsPointInBoundsAndValid(InPoint))
	{
		return InPoint.X + InPoint.Y * InventorySize.X;
	}
	else
	{
		UE_LOGFMT(LogHGame, Error, "Could not convert index for point {point}", InPoint.ToString());
		return INDEX_NONE;
	}
	
}

FHInventoryPoint UHGridInventoryComponent::IndexToInventoryPoint(int32 Index) const
{
	if(Index != INDEX_NONE && Index < LocalGridArray.Num())
	{
		return FHInventoryPoint{ Index % InventorySize.X, Index / InventorySize.X };
	}
	else
	{
		UE_LOGFMT(LogHGame, Error, "Could not convert index: {idx}, with inv size: {size}", Index, InventorySize.ToString());
		return FHInventoryPoint{};
	}
	
}

bool UHGridInventoryComponent::IsPointInBoundsAndValid(const FHInventoryPoint& InPoint) const
{
	//Implicitly performs IsValid on point because invalid is 255
	return ( (InPoint.X < InventorySize.X) && (InPoint.Y < InventorySize.Y) );
}

bool UHGridInventoryComponent::IsItemInBoundsAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const
{
	if(GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		const FHInventoryPoint ItemSize = GridEntry->GetCurrentDimensions();
		return ((InPoint.X + ItemSize.X < InventorySize.X) && (InPoint.Y + ItemSize.Y < InventorySize.Y));
	}

	return false;
	
}

bool UHGridInventoryComponent::IsFreeRoomAvailableAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry) const
{
	if(GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		TArray<UHLocalGridEntry*> BlockingItems;

		if(GetAllBlockingItemsAtPoint(InPoint, GridEntry, BlockingItems))
		{
			if(BlockingItems.IsEmpty())
			{
				return true;
			}
		}
	}

	return false;
}

bool UHGridInventoryComponent::GetAllBlockingItemsAtPoint(const FHInventoryPoint& InPoint, UHLocalGridEntry* GridEntry,
                                                   TArray<UHLocalGridEntry*>& OutBlockingItems) const
{
	OutBlockingItems.Reset();

	if (GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		TArray<int32> ItemIndices;
		GetCurrentItemIndices(GridEntry, ItemIndices);

		for (int32 Index : ItemIndices)
		{
			UHLocalGridEntry* Entry = GetItemAtIndex(Index);

			if(Entry && Entry != GridEntry)
			{
				OutBlockingItems.AddUnique(Entry);
			}
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::GetCurrentInventoryPoints(const UHLocalGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const
{
	if (GridEntry && IsPointInBoundsAndValid(GridEntry->TopLeftTilePoint))
	{
		return GetCurrentInventoryPointsAtPoint(GridEntry->TopLeftTilePoint, GridEntry, OutPoints);
	}
	return false;
}

bool UHGridInventoryComponent::GetCurrentInventoryPointsAtPoint(const FHInventoryPoint& InPoint,
	const UHLocalGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const
{
	OutPoints.Reset();

	if (GridEntry && IsPointInBoundsAndValid(GridEntry->TopLeftTilePoint))
	{
		const FHInventoryPoint CurrentDimensions = GridEntry->GetCurrentDimensions();
		FHInventoryPoint CurrentPoint = FHInventoryPoint();
		for (int i = 0; i < CurrentDimensions.X; i++)
		{
			CurrentPoint.X = GridEntry->TopLeftTilePoint.X + i;

			for (int j = 0; j < CurrentDimensions.Y; j++)
			{
				CurrentPoint.Y = GridEntry->TopLeftTilePoint.Y + j;

				OutPoints.Add(CurrentPoint);
			}
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::GetCurrentItemIndices(const UHLocalGridEntry* GridEntry, TArray<int32>& OutIndices) const
{
	
	if (GridEntry && IsPointInBoundsAndValid(GridEntry->TopLeftTilePoint))
	{
		return GetCurrentItemIndicesAtPoint(GridEntry->TopLeftTilePoint, GridEntry, OutIndices);
	}
	return false;
}

bool UHGridInventoryComponent::GetCurrentItemIndicesAtPoint(const FHInventoryPoint& InPoint, const UHLocalGridEntry* GridEntry,
                                                            TArray<int32>& OutIndices) const
{
	OutIndices.Reset();  // Clear the output array

	if (GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		const FHInventoryPoint CurrentDimensions = GridEntry->GetCurrentDimensions();

		int32 InitialIndex = InventoryPointToIndex(InPoint);
		for (int i = 0; i < CurrentDimensions.X; i++)
		{
			for (int j = 0; j < CurrentDimensions.Y; j++)
			{
				int32 CurrentIndex = InitialIndex + i + j * InventorySize.X;

				OutIndices.Add(CurrentIndex);
			}
		}

		return true;
	}

	return false;
}

bool UHGridInventoryComponent::RefreshLocalEntry(UHLocalGridEntry* LocalEntry, bool& bOutPositionChanged)
{
	FHInventoryEntry StructEntry = FHInventoryEntry();

	if(FindStructEntryByID(LocalEntry->LinkedRepID, StructEntry))
	{
		LocalEntry->UpdateData(StructEntry, bOutPositionChanged);
		return true;
	}

	return false;
}

UHLocalGridEntry* UHGridInventoryComponent::FindEntryInGridByIDSmart(const int32 ItemID)
{
	const FHInventoryPoint InventorySize = GetInventorySize();

	for (int i = 0; i < LocalGridArray.Num();)
	{
		UHLocalGridEntry* Entry = GetItemAtIndex(i);

		if(Entry)
		{
			if(Entry->LinkedRepID == ItemID)
			{
				return Entry;
			}

			i += Entry->GetCurrentDimensions().X;
		}
		else
		{
			i++;
		}
	}

	UE_LOGFMT(LogHGame, Error, "Could not find item by ID, {id} in local grid", ItemID);

	return nullptr;
}

bool UHGridInventoryComponent::FindStructEntryByID(int32 ItemID, FHInventoryEntry& OutEntry)
{
	for (const FHInventoryEntry& Entry : MasterList)
	{
		if (Entry.ReplicationID == ItemID)
		{
			OutEntry = Entry;
			return true;
		}
	}

	UE_LOGFMT(LogHGame, Error, "Could not find struct entry by ID, {id} in InventoryList", ItemID);

	return false;
}

UHLocalGridEntry* UHGridInventoryComponent::GetItemAtPoint(const FHInventoryPoint& InPoint)
{
	if(IsPointInBoundsAndValid(InPoint))
	{
		return	GetItemAtIndex(InventoryPointToIndex(InPoint));
	}

	return nullptr;
}

UHLocalGridEntry* UHGridInventoryComponent::GetItemAtIndex(int32 Index) const
{
	if (Index >= 0 && Index < LocalGridArray.Num())
	{
		return LocalGridArray[Index];
	}

	return nullptr;
}

bool UHGridInventoryComponent::SetItemAtIndex(int32 Index, UHLocalGridEntry* Entry)
{
	if (Index >= 0 && Index < LocalGridArray.Num())
	{
		LocalGridArray[Index] = Entry;
		return true;
	}

	return false;
}

void UHGridInventoryComponent::OnRep_InventorySize()
{
	UE_LOGFMT(LogHGame, Warning, "OnRep: Inventory Size has changed. Should have logic here");
}

