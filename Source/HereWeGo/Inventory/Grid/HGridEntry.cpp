// Fill out your copyright notice in the Description page of Project Settings.


#include "HGridEntry.h"

#include "HInventoryItemInstance.h"

UHGridEntry::UHGridEntry()
{
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

bool UHGridEntry::CanStackWith(UHGridEntry* OtherEntry)
{
	if (OtherEntry->Instance && Instance->IsItemStackCompatible(OtherEntry->Instance))
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

int32 UHGridEntry::AddStackCountSafe(int32 CountToAdd)
{
	if (CountToAdd < 1)
	{
		return 0;
	}

	int32 OldCount = StackCount;
	int32 NewCount = StackCount + CountToAdd;
	int32 MaxStack = GetMaxStackCount();

	if (NewCount > MaxStack)
	{
		NewCount = MaxStack;
		CountToAdd = NewCount - OldCount;
	}

	SetStackCount(NewCount);
	return CountToAdd;
}

void UHGridEntry::DecrementStackCount(int32 CountToRemove)
{
	int32 NewCount = StackCount - CountToRemove;

	if (NewCount < 0)
	{
		NewCount = 0;
	}

	SetStackCount(NewCount);
}

int32 UHGridEntry::GetReplicatedID() const
{
	return LinkedRepID;
}

void UHGridEntry::SetReplicatedID(int32 RepID)
{
	LinkedRepID = RepID;
}

int32 UHGridEntry::GetStackCount()
{
	return StackCount;
}

void UHGridEntry::SetStackCount(int32 NewCount)
{
	{
		StackCount = NewCount;
		//Broadcast a change here
	}
}

int32 UHGridEntry::GetMaxStackCount()
{
	if (Instance)
	{
		return Instance->GetMaxStack();
	}

	return INDEX_NONE;
}

FHInventoryPoint UHGridEntry::GetCurrentDimensions() const
{
	FHInventoryPoint UnrotatedItemDimensions = Instance->GetItemDimensions();
	return (IsRotated ? FHInventoryPoint(UnrotatedItemDimensions.Y, UnrotatedItemDimensions.X) : UnrotatedItemDimensions);
}

bool UHGridEntry::GetCanItemBeStacked()
{
	return Instance->GetCanBeStacked();
}
