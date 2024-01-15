// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryGrid.h"

#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

UHGridEntry::UHGridEntry()
{
}

FHInventoryPoint UHGridEntry::GetCurrentDimensions() const
{
	FHInventoryPoint UnrotatedItemDimensions = Instance->GetItemDimensions();
	return (IsRotated ? FHInventoryPoint(UnrotatedItemDimensions.Y, UnrotatedItemDimensions.X) : UnrotatedItemDimensions);
}

void UHGridEntry::Initialize(const FHInventoryEntry& Entry)
{
	Instance = Entry.Instance;

	TopLeftTilePoint = Entry.TopLeftTilePoint;

	IsRotated = Entry.IsRotated;

	int32 StackCount = Entry.StackCount;

	int32 LinkedRepID = Entry.ReplicationID;
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
	int32 StackCount = Entry.StackCount;
	int32 LinkedRepID = Entry.ReplicationID;
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
				UE_LOGFMT(LogHGame, Error, "There is a conflict in position {x} between {item1} and {item2}. Breaking out this shouldnt happen", Index, LocalGridArray[Index], EntryToAdd);
				break;
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
	NewLocalEntry->Initialize(EntryToAdd);
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
	if (Index != INDEX_NONE && Index < LocalGridArray.Num())
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

bool UHGridArray::IsFreeRoomAvailableAtPoint(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry) const
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

bool UHGridArray::GetAllBlockingItemsAtPoint(const FHInventoryPoint& InPoint, UHGridEntry* GridEntry,
	TArray<UHGridEntry*>& OutBlockingItems) const
{
	OutBlockingItems.Reset();

	if (GridEntry && IsPointInBoundsAndValid(InPoint))
	{
		TArray<int32> ItemIndices;
		GetCurrentItemIndices(GridEntry, ItemIndices);

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

bool UHGridArray::GetCurrentItemIndicesAtPoint(const FHInventoryPoint& InPoint, const UHGridEntry* GridEntry,
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

bool UHGridArray::RefreshLocalEntry(UHGridEntry* LocalEntry, bool& bOutPositionChanged)
{
	FHInventoryEntry StructEntry = FHInventoryEntry();

	if (FindStructEntryByID(LocalEntry->LinkedRepID, StructEntry))
	{
		LocalEntry->UpdateData(StructEntry, bOutPositionChanged);
		return true;
	}

	return false;
}

UHGridEntry* UHGridArray::FindEntryInGridByIDSmart(const int32 ItemID)
{
	const FHInventoryPoint InventorySize = GetInventorySize();

	for (int i = 0; i < LocalGridArray.Num();)
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
	if (Index >= 0 && Index < LocalGridArray.Num())
	{
		return LocalGridArray[Index];
	}

	return nullptr;
}

bool UHGridArray::SetItemAtIndex(int32 Index, UHGridEntry* Entry)
{
	if (Index >= 0 && Index < LocalGridArray.Num())
	{
		LocalGridArray[Index] = Entry;
		return true;
	}

	return false;
}
