// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryGrid.h"

#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

UHGridItem::UHGridItem()
{
}

UHGridItem::UHGridItem(UHInventoryItemInstance* InInstance)
{
	Instance = InInstance;
}

FHInventoryPoint UHGridItem::GetCurrentDimensions() const
{
	FHInventoryPoint UnrotatedItemDimensions = Instance->GetItemDimensions();
	return (IsRotated ? FHInventoryPoint(UnrotatedItemDimensions.Y, UnrotatedItemDimensions.X) : UnrotatedItemDimensions);
}

void UHGridItem::SetReplicatedID(int32 RepID)
{
	LinkedRepID = RepID;
}

void UHGridItem::LoadEntryData(const FHInventoryEntry& Entry)
{
	Instance = Entry.Instance;

	TopLeftTilePoint = Entry.TopLeftTilePoint;

	IsRotated = Entry.IsRotated;

	int32 StackCount = Entry.StackCount;

	int32 LinkedRepID = Entry.ReplicationID;
}

void UHGridItem::UpdateData(const FHInventoryEntry& Entry, bool& bOutPositionChanged)
{
	bOutPositionChanged = false;
	if (Entry.IsRotated != IsRotated || Entry.TopLeftTilePoint != TopLeftTilePoint)
	{
		bOutPositionChanged = true;
	}

	Instance = Entry.Instance;
	TopLeftTilePoint = Entry.TopLeftTilePoint;
	IsRotated = Entry.IsRotated;
	int32 StackCount = Entry.StackCount;
	int32 LinkedRepID = Entry.ReplicationID;
}

void UHGridItem::InitializeData(UHInventoryItemInstance* ItemInstance, FHInventoryPoint Point, bool bIsRotated,
	int32 InStackCount)
{
	Instance = ItemInstance;
	TopLeftTilePoint = Point;
	IsRotated = bIsRotated;
	StackCount = InStackCount;
}

int32 UHGridItem::GetMaxStackCount()
{
	if(Instance)
	{
		return Instance->GetMaxStack();
	}

	return INDEX_NONE;
}

bool UHGridItem::GetCanItemBeStacked()
{
	return Instance->GetCanBeStacked();
}

bool UHGridItem::CanStackWith(UHGridItem* OtherEntry)
{
	if(OtherEntry->Instance && Instance->IsItemStackCompatible(OtherEntry->Instance))
	{
		return true;
	}
	return false;
}

bool UHGridItem::CanStackWith(UHInventoryItemInstance* ItemInstance)
{
	if (ItemInstance && Instance->IsItemStackCompatible(ItemInstance))
	{
		return true;
	}
	return false;
}

//Attempts to add to stack if instance matches.
//Returns amount added
//Returns 0 if error
int32 UHGridItem::TryToAddInstanceStack(UHInventoryItemInstance* IncomingInstance, int32 StackCountToAdd)
{
	if(CanStackWith(IncomingInstance))
	{
		int32 StacksAdded = AddStackCountSafe(StackCountToAdd);
		return StacksAdded;
	}

	return 0;
}

bool UHGridArray::RemoveItemFromGrid(UHGridItem* EntryToRemove)
{
	bIsDirty = true;
	TArray<int32> ItemIndicesToRemove;

	if (GetCurrentItemIndices(EntryToRemove, ItemIndicesToRemove))
	{
		for (int32 Index : ItemIndicesToRemove)
		{
			UHGridItem* OccupyingEntry = GetItemAtIndex(Index);
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

bool UHGridArray::AddItemToGrid(UHGridItem* EntryToAdd) //todo not done
{
	TArray<int32> ItemIndicesToAdd;

	if (GetCurrentItemIndices(EntryToAdd, ItemIndicesToAdd))
	{
		for (int32 Index : ItemIndicesToAdd)
		{
			UHGridItem* ObjectAtIndex = GetItemAtIndex(Index);

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
	FHInventoryEntry Entry;

	if (FindStructEntryByID(ItemID, Entry))
	{
		OutPoint = Entry.TopLeftTilePoint;

		return true;
	}

	return false;
}

//void UHGridArray::ForcePendingGridUpdate()
//{
//	UE_LOGFMT(LogHGame, Warning, "Forcing early grid update");
//	UpdatePendingEntryPositionsGrid();
//}

void UHGridArray::UpdatePendingEntryPositionsGrid()
{
	for (UHGridItem* Entry : LocalPendingItemsToMove)
	{
		RemoveItemFromGrid(Entry);
	}

	for (UHGridItem* Entry : LocalPendingItemsToMove)
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

	UHGridItem* EntryToRemove = nullptr;

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

	UHGridItem* NewLocalEntry = NewObject<UHGridItem>();
	NewLocalEntry->LoadEntryData(EntryToAdd);
	LocalPendingItemsToMove.AddUnique(NewLocalEntry);
}

void UHGridArray::HandlePostChange(FHInventoryEntry& Entry)
{
	bIsDirty = true;

	UHGridItem* EntryToUpdate = nullptr;

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

bool UHGridArray::IsItemInBoundsAtPoint(const FHInventoryPoint& InPoint, UHGridItem* GridEntry) const
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
			GetCurrentItemIndicesAtPoint()
		}
	}

	return false;
}

bool UHGridArray::IsFreeRoomAvailableAtPointForEntry(const FHInventoryPoint& InPoint, UHGridItem* GridEntry) const
{
	if (GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		TArray<UHGridItem*> BlockingItems;

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
		UHGridItem* OccupyingEntry = GetItemAtIndex(i);

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

bool UHGridArray::FindNextBestSlotPoint(UHGridItem* Entry, FHInventoryPoint& OutPoint) const
{
	if(!Entry)
	{
		return false;
	}

	bool bIncomingEntryCanStack = Entry->GetCanItemBeStacked();

	for (int i = 0; i < GridArray.Num(); i++)
	{
		UHGridItem* OccupyingEntry = GetItemAtIndex(i);

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

UHGridItem* UHGridArray::AddItemInstanceToGridAtPoint(const FHInventoryPoint& InPoint,
	UHInventoryItemInstance* ItemInstance, int32 StackCount)
{
	if(IsFreeRoomAvailableAtPointWithSize(InPoint, ItemInstance->GetItemDimensions()))
	{
		UHGridItem* NewItem = NewObject<UHGridItem>();
		NewItem->InitializeData(ItemInstance, InPoint, false, StackCount);
		if(AddItemToGrid(NewItem))
		{
			
		}
	}
}

bool UHGridArray::GetAllBlockingItemsAtPoint(const FHInventoryPoint& InPoint, UHGridItem* GridEntry,
                                             TArray<UHGridItem*>& OutBlockingItems) const
{
	OutBlockingItems.Reset();

	if (GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		TArray<int32> ItemIndices;
		GetCurrentItemIndicesAtPoint(InPoint, GridEntry, ItemIndices);

		for (int32 Index : ItemIndices)
		{
			UHGridItem* Entry = GetItemAtIndex(Index);

			if (Entry && Entry != GridEntry)
			{
				OutBlockingItems.AddUnique(Entry);
			}
		}

		return true;
	}

	return false;
}

bool UHGridArray::GetCurrentInventoryPoints(const UHGridItem* GridEntry, TArray<FHInventoryPoint>& OutPoints) const
{
	if (GridEntry && IsPointInBoundsAndValid(GridEntry->TopLeftTilePoint))
	{
		return GetCurrentInventoryPointsAtPoint(GridEntry->TopLeftTilePoint, GridEntry, OutPoints);
	}
	return false;
}

bool UHGridArray::GetCurrentInventoryPointsAtPoint(const FHInventoryPoint& InPoint,
	const UHGridItem* GridEntry, TArray<FHInventoryPoint>& OutPoints) const
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

bool UHGridArray::GetCurrentItemIndices(const UHGridItem* GridEntry, TArray<int32>& OutIndices) const
{

	if (GridEntry && IsPointInBoundsAndValid(GridEntry->TopLeftTilePoint))
	{
		return GetCurrentItemIndicesAtPoint(GridEntry->TopLeftTilePoint, GridEntry, OutIndices);
	}
	return false;
}

bool UHGridArray::GetCurrentItemIndicesAtPoint(const FHInventoryPoint& InPoint, const UHGridItem* GridEntry, TArray<int32>& OutIndices) const
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

bool UHGridArray::RefreshLocalEntry(UHGridItem* LocalEntry, bool& bOutPositionChanged)
{
	FHInventoryEntry StructEntry = FHInventoryEntry();

	if (FindStructEntryByID(LocalEntry->LinkedRepID, StructEntry))
	{
		LocalEntry->UpdateData(StructEntry, bOutPositionChanged);
		return true;
	}

	return false;
}

UHGridItem* UHGridArray::FindEntryInGridByIDSmart(const int32 ItemID)
{
	const FHInventoryPoint InventorySize = GetInventorySize();

	for (int i = 0; i < GridArray.Num();)
	{
		UHGridItem* Entry = GetItemAtIndex(i);

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

bool UHGridArray::FindStructEntryByID(int32 ItemID, FHInventoryEntry& OutEntry)
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

UHGridItem* UHGridArray::GetItemAtPoint(const FHInventoryPoint& InPoint)
{
	if (IsPointInBoundsAndValid(InPoint))
	{
		return	GetItemAtIndex(InventoryPointToIndex(InPoint));
	}

	return nullptr;
}

UHGridItem* UHGridArray::GetItemAtIndex(int32 Index) const
{
	if (Index >= 0 && Index < GridArray.Num())
	{
		return GridArray[Index];
	}

	return nullptr;
}

bool UHGridArray::SetItemAtIndex(int32 Index, UHGridItem* Entry)
{
	if (Index >= 0 && Index < GridArray.Num())
	{
		GridArray[Index] = Entry;
		return true;
	}

	return false;
}


