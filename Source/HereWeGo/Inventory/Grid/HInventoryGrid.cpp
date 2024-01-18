// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryGrid.h"

#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

UHGridEntry::UHGridEntry()
{
}

UHGridEntry::UHGridEntry(UHInventoryItemInstance* InInstance)
{
	Instance = InInstance;
}

FHInventoryPoint UHGridEntry::GetCurrentDimensions() const
{
	FHInventoryPoint UnrotatedItemDimensions = Instance->GetItemDimensions();
	return (IsRotated ? FHInventoryPoint(UnrotatedItemDimensions.Y, UnrotatedItemDimensions.X) : UnrotatedItemDimensions);
}

void UHGridEntry::SetReplicatedID(int32 RepID)
{
	LinkedRepID = RepID;
}

void UHGridEntry::LoadEntryData(const FHInventoryEntry& Entry)
{
	Instance = Entry.Instance;

	TopLeftTilePoint = Entry.TopLeftTilePoint;

	IsRotated = Entry.IsRotated;

	StackCount = Entry.StackCount;

	LinkedRepID = Entry.ReplicationID;
}

void UHGridEntry::UpdateData(const FHInventoryEntry& Entry, bool& bOutPositionChanged)
{
	bOutPositionChanged = false;
	if (Entry.IsRotated != IsRotated || Entry.TopLeftTilePoint != TopLeftTilePoint)
	{
		bOutPositionChanged = true;
	}

	Instance = Entry.Instance;
	TopLeftTilePoint = Entry.TopLeftTilePoint;
	IsRotated = Entry.IsRotated;
	StackCount = Entry.StackCount;
	LinkedRepID = Entry.ReplicationID;
}

void UHGridEntry::InitializeData(UHInventoryItemInstance* ItemInstance, FHInventoryPoint Point, bool bIsRotated,
	int32 InStackCount)
{
	Instance = ItemInstance;
	TopLeftTilePoint = Point;
	IsRotated = bIsRotated;
	StackCount = InStackCount;
}

int32 UHGridEntry::GetMaxStackCount()
{
	if(Instance)
	{
		return Instance->GetMaxStack();
	}

	return INDEX_NONE;
}

bool UHGridEntry::GetCanItemBeStacked()
{
	return Instance->GetCanBeStacked();
}

bool UHGridEntry::CanStackWith(UHGridEntry* OtherEntry)
{
	if(OtherEntry->Instance && Instance->IsItemStackCompatible(OtherEntry->Instance))
	{
		return true;
	}
	return false;
}

bool UHGridEntry::CanStackWith(UHInventoryItemInstance* ItemInstance)
{
	if (ItemInstance && Instance->IsItemStackCompatible(ItemInstance))
	{
		return true;
	}
	return false;
}

bool UHGridArray::RemoveItemFromGrid(UHGridEntry* EntryToRemove)
{
	bIsDirty = true;
	TArray<int32> ItemIndicesToRemove;

	if (GetCurrentItemIndices(EntryToRemove, ItemIndicesToRemove))
	{
		for (int32 Index : ItemIndicesToRemove)
		{
			UHGridEntry* OccupyingEntry = GetItemAtIndex(Index);
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

bool UHGridArray::AddItemToGrid(UHGridEntry* EntryToAdd) //todo not done
{
	TArray<int32> ItemIndicesToAdd;

	if (GetCurrentItemIndices(EntryToAdd, ItemIndicesToAdd))
	{
		for (int32 Index : ItemIndicesToAdd)
		{
			UHGridEntry* ObjectAtIndex = GetItemAtIndex(Index);

			if (ObjectAtIndex != nullptr && ObjectAtIndex != EntryToAdd)
			{
				UE_LOGFMT(LogHGame, Error, "There is a conflict in position {x} between {item1} and {item2}. Breaking out this shouldnt happen", Index, GridArray[Index], EntryToAdd);
				return false;
			}

			SetItemAtIndex(Index, EntryToAdd);
		}

		return true;
	}

	return false;
}

bool UHGridArray::GetEntrySlotPointFromID(int32 ItemID, FHInventoryPoint& OutPoint)
{
	/*FHInventoryEntry Entry;

	if (FindStructEntryByID(ItemID, Entry))
	{
		OutPoint = Entry.TopLeftTilePoint;

		return true;
	}*/

	return false;
}

//void UHGridArray::ForcePendingGridUpdate()
//{
//	UE_LOGFMT(LogHGame, Warning, "Forcing early grid update");
//	UpdatePendingEntryPositionsGrid();
//}

void UHGridArray::UpdatePendingEntryPositionsGrid()
{
	for (UHGridEntry* Entry : LocalPendingItemsToMove)
	{
		RemoveItemFromGrid(Entry);
	}

	for (UHGridEntry* Entry : LocalPendingItemsToMove)
	{
		AddItemToGrid(Entry);
	}

	LocalPendingItemsToMove.Reset();
	bIsDirty = false;
}

UHGridArray::UHGridArray()
{
}

FHInventoryPoint UHGridArray::GetInventorySize()
{
	return InventorySize;
}

void UHGridArray::HandlePreRemove(FHInventoryEntry& Entry)
{
	bIsDirty = true;

	UHGridEntry* EntryToRemove = nullptr;

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

void UHGridArray::HandlePostAdd(FHInventoryEntry& EntryToAdd)
{
	bIsDirty = true;

	UHGridEntry* NewLocalEntry = NewObject<UHGridEntry>();
	NewLocalEntry->LoadEntryData(EntryToAdd);
	LocalPendingItemsToMove.AddUnique(NewLocalEntry);
}

void UHGridArray::HandlePostChange(FHInventoryEntry& Entry)
{
	bIsDirty = true;

	UHGridEntry* EntryToUpdate = nullptr;

	EntryToUpdate = FindEntryInGridByIDSmart(Entry.ReplicationID);

	if (EntryToUpdate)
	{
		bool bPositionChanged;
		EntryToUpdate->UpdateData(Entry, bPositionChanged);

		if (bPositionChanged)
		{
			LocalPendingItemsToMove.AddUnique(EntryToUpdate);
		}
	}
	else
	{
		UE_LOG(LogHGame, Error, TEXT("Failed to update item in removelocalitems()"));
	}
}

int32 UHGridArray::InventoryPointToIndex(const FHInventoryPoint& InPoint) const
{
	if (IsPointInBoundsAndValid(InPoint))
	{
		return InPoint.X + InPoint.Y * InventorySize.X;
	}
	else
	{
		UE_LOGFMT(LogHGame, Error, "Could not convert index for point {point}", InPoint.ToString());
		return INDEX_NONE;
	}

}

FHInventoryPoint UHGridArray::IndexToInventoryPoint(int32 Index) const
{
	if (Index != INDEX_NONE && Index < GridArray.Num())
	{
		return FHInventoryPoint{ Index % InventorySize.X, Index / InventorySize.X };
	}
	else
	{
		UE_LOGFMT(LogHGame, Error, "Could not convert index: {idx}, with inv size: {size}", Index, InventorySize.ToString());
		return FHInventoryPoint{};
	}

}

bool UHGridArray::IsPointInBoundsAndValid(const FHInventoryPoint& InPoint) const
{
	//Implicitly performs IsValid on point because invalid is 255
	return ((InPoint.X < InventorySize.X) && (InPoint.Y < InventorySize.Y));
}

bool UHGridArray::IsItemInBoundsAtPoint(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry) const
{
	if (GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		const FHInventoryPoint ItemSize = GridEntry->GetCurrentDimensions();
		return ((InPoint.X + ItemSize.X < InventorySize.X) && (InPoint.Y + ItemSize.Y < InventorySize.Y));
	}

	return false;

}

bool UHGridArray::IsFreeRoomAvailableAtPointWithSize(const FHInventoryPoint& InPoint, const FHInventoryPoint& InSize) const
{
	if (InSize.IsValid() && IsPointInBoundsAndValid(InPoint))
	{
		if(IsPointInBoundsAndValid(InPoint + InSize)) //Check that largest point is in bounds
		{
			int32 InitialIndex = InventoryPointToIndex(InPoint);
			for (int i = 0; i < InSize.X; i++)
			{
				for (int j = 0; j < InSize.Y; j++)
				{
					int32 CurrentIndex = InitialIndex + i + j * InventorySize.X;

					if(GetItemAtIndex(CurrentIndex))
					{
						UE_LOGFMT(LogHGame, Warning, "Free Room is not available at point with size");
						return false;
					}
				}
			}

			return true;
		}
	}

	return false;
}

bool UHGridArray::IsFreeRoomAvailableAtPointForEntry(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry) const
{
	if (GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		TArray<UHGridEntry*> BlockingItems;

		if (GetAllBlockingItemsAtPoint(InPoint, GridEntry, BlockingItems))
		{
			if (BlockingItems.IsEmpty())
			{
				return true;
			}
		}
	}

	return false;
}

bool UHGridArray::FindNextSlotPointForInstance(UHInventoryItemInstance* IncomingInstance, FHInventoryPoint& OutPoint) const
{
	if (!IncomingInstance)
	{
		return false;
	}

	bool bIncomingItemCanStack = IncomingInstance->GetCanBeStacked();

	for (int i = 0; i < GridArray.Num(); i++)
	{
		UHGridEntry* OccupyingEntry = GetItemAtIndex(i);

		if (OccupyingEntry)
		{
			if (bIncomingItemCanStack && OccupyingEntry->Instance->IsItemStackCompatible(IncomingInstance))
			{
				OutPoint = OccupyingEntry->TopLeftTilePoint;
				return true;
			}
			i += OccupyingEntry->GetCurrentDimensions().X;
		}
		else
		{
			if (IsFreeRoomAvailableAtPointWithSize(IndexToInventoryPoint(i), IncomingInstance->GetItemDimensions()))
			{
				OutPoint = IndexToInventoryPoint(i);
				return true;
			}
		}
	}

	return false;
}

bool UHGridArray::FindNextBestSlotPoint(UHGridEntry* Entry, FHInventoryPoint& OutPoint) const
{
	if(!Entry)
	{
		return false;
	}

	bool bIncomingEntryCanStack = Entry->GetCanItemBeStacked();

	for (int i = 0; i < GridArray.Num(); i++)
	{
		UHGridEntry* OccupyingEntry = GetItemAtIndex(i);

		if (OccupyingEntry)
		{
			if(bIncomingEntryCanStack && OccupyingEntry->CanStackWith(Entry))
			{
				OutPoint = OccupyingEntry->TopLeftTilePoint;
				return true;
			}
			i += OccupyingEntry->GetCurrentDimensions().X;
		}
		else
		{
			if(IsFreeRoomAvailableAtPointForEntry(IndexToInventoryPoint(i), Entry))
			{
				OutPoint = IndexToInventoryPoint(i);
				return true;
			}
		}
	}

	return false;
}

UHGridEntry* UHGridArray::AddItemInstanceToGridAtPoint(const FHInventoryPoint& InPoint,
	UHInventoryItemInstance* ItemInstance, int32 StackCount)
{
	if(IsFreeRoomAvailableAtPointWithSize(InPoint, ItemInstance->GetItemDimensions()))
	{
		UHGridEntry* NewItem = NewObject<UHGridEntry>();
		NewItem->InitializeData(ItemInstance, InPoint, false, StackCount);
		if(AddItemToGrid(NewItem))
		{
			return NewItem;
		}
	}

	return nullptr;
}

bool UHGridArray::GetAllBlockingItemsAtPoint(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry,
                                             TArray<UHGridEntry*>& OutBlockingItems) const
{
	OutBlockingItems.Reset();

	if (GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		TArray<int32> ItemIndices;
		GetCurrentItemIndicesAtPoint(InPoint, GridEntry, ItemIndices);

		for (int32 Index : ItemIndices)
		{
			UHGridEntry* Entry = GetItemAtIndex(Index);

			if (Entry && Entry != GridEntry)
			{
				OutBlockingItems.AddUnique(Entry);
			}
		}

		return true;
	}

	return false;
}

bool UHGridArray::GetCurrentInventoryPoints(const UHGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const
{
	if (GridEntry && IsPointInBoundsAndValid(GridEntry->TopLeftTilePoint))
	{
		return GetCurrentInventoryPointsAtPoint(GridEntry->TopLeftTilePoint, GridEntry, OutPoints);
	}
	return false;
}

bool UHGridArray::GetCurrentInventoryPointsAtPoint(const FHInventoryPoint& InPoint,
	const UHGridEntry* GridEntry, TArray<FHInventoryPoint>& OutPoints) const
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

bool UHGridArray::GetCurrentItemIndices(const UHGridEntry* GridEntry, TArray<int32>& OutIndices) const
{

	if (GridEntry && IsPointInBoundsAndValid(GridEntry->TopLeftTilePoint))
	{
		return GetCurrentItemIndicesAtPoint(GridEntry->TopLeftTilePoint, GridEntry, OutIndices);
	}
	return false;
}

bool UHGridArray::GetCurrentItemIndicesAtPoint(const FHInventoryPoint& InPoint, const UHGridEntry* GridEntry, TArray<int32>& OutIndices) const
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

bool UHGridArray::GetIndicesForSizeAtPoint(const FHInventoryPoint& InPoint, const FHInventoryPoint& InSize,
	TArray<int32>& OutIndices) const
{
	OutIndices.Reset();  // Clear the output array

	if (InSize.IsValid() && IsPointInBoundsAndValid(InPoint))
	{
		int32 InitialIndex = InventoryPointToIndex(InPoint);

		for (int i = 0; i < InSize.X; i++)
		{
			for (int j = 0; j < InSize.Y; j++)
			{
				int32 CurrentIndex = InitialIndex + i + j * InventorySize.X;

				OutIndices.Add(CurrentIndex);
			}
		}

		return true;
	}

	return false;
}

bool UHGridArray::RefreshLocalEntry(UHGridEntry* LocalEntry, bool& bOutPositionChanged)
{
	/*FHInventoryEntry StructEntry = FHInventoryEntry();

	if (FindStructEntryByID(LocalEntry->LinkedRepID, StructEntry))
	{
		LocalEntry->UpdateData(StructEntry, bOutPositionChanged);
		return true;
	}*/
	UE_LOGFMT(LogHGame, Error, "refresh lcoal entry is not implemented returnign false;");

	return false;
}

UHGridEntry* UHGridArray::FindEntryInGridByIDSmart(const int32 ItemID)
{
	for (int i = 0; i < GridArray.Num();)
	{
		UHGridEntry* Entry = GetItemAtIndex(i);

		if (Entry)
		{
			if (Entry->LinkedRepID == ItemID)
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

UHGridEntry* UHGridArray::GetItemAtPoint(const FHInventoryPoint& InPoint)
{
	if (IsPointInBoundsAndValid(InPoint))
	{
		return	GetItemAtIndex(InventoryPointToIndex(InPoint));
	}

	return nullptr;
}

UHGridEntry* UHGridArray::GetItemAtIndex(int32 Index) const
{
	if (Index >= 0 && Index < GridArray.Num())
	{
		return GridArray[Index];
	}

	return nullptr;
}

bool UHGridArray::SetItemAtIndex(int32 Index, UHGridEntry* Entry)
{
	if (Index >= 0 && Index < GridArray.Num())
	{
		GridArray[Index] = Entry;
		return true;
	}

	return false;
}

bool UHGridArray::CanEntriesStack(UHGridEntry* BaseEntry, UHGridEntry* StackingEntry)
{
	return false;
}

int32 UHGridArray::TryToAddInstanceStackToEntry(UHGridEntry* EntryToAddTo, UHInventoryItemInstance* IncomingInstance, int32 StackCountToAdd)
{
	if (EntryToAddTo->CanStackWith(IncomingInstance))
	{
		int32 StacksAdded = EntryToAddTo->AddStackCountSafe(StackCountToAdd);
		return StacksAdded;
	}

	return 0;
}

int32 UHGridArray::TryToStackEntries(UHGridEntry* EntryToAddTo, UHGridEntry* RequestingEntry)
{
	if(!EntryToAddTo || !RequestingEntry)
	{
		return 0;
	}

	if (EntryToAddTo->CanStackWith(RequestingEntry))
	{
		int32 StacksAdded = EntryToAddTo->AddStackCountSafe(RequestingEntry->StackCount);
		RequestingEntry->DecrementStackCount(StacksAdded);
		return StacksAdded;
	}

	return 0;
}
