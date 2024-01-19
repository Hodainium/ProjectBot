// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryComponent.h"
#include "HItemDefinition.h"
#include "HInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/GameplayMessageSubsystem.h"
//#include "HereWeGo/Inventory/Grid/HInventoryGrid.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Inventory_Message_StackChanged, "Inventory.Message.StackChanged");
//UE_DEFINE_GAMEPLAY_TAG(TAG_Inventory_Item_Count, "ItemStatTags.Inventory.Item.Count");

//FHInventoryEntry::FHInventoryEntry(UHGridEntry* Item)
//{
//	if (Item)
//	{
//		Instance = Item->Instance;
//		TopLeftTilePoint = Item->TopLeftTilePoint;
//		IsRotated = Item->IsRotated;
//		StackCount = Item->StackCount;
//	}
//}

FString FHInventoryEntry::GetDebugString() const
{
	UHItemDefinition* ItemDef = nullptr;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDefinition();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

TArray<UHInventoryItemInstance*> FHInventoryList::GetAllItems() const
{
	TArray<UHInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FHInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

void FHInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FHInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FHInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FHInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FHInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FHInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FHInventoryList::PostReplicatedReceive(const FPostReplicatedReceiveParameters& Parameters)
{

}

void FHInventoryList::RemoveEntry(UHInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FHInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

bool FHInventoryList::MarkItemIDDirty(int32 ItemID)
{
	for (FHInventoryEntry& Entry :Entries)
	{
		if(Entry.ReplicationID == ItemID)
		{
			MarkItemDirty(Entry);
			return true;
		}
	}
	return false;
}

void FHInventoryList::BroadcastChangeMessage(FHInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FHInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	//Entry.Instance->AddStatTagStack(TAG_Inventory_Item_Count, NewCount);

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Inventory_Message_StackChanged, Message);
}

UHInventoryItemInstance* FHInventoryList::AddEntry(UHItemDefinition* ItemDef, int32 StackCount)
{
	UHInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FHInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UHInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);
	for (UHInventoryItemFragment* Fragment : ItemDef->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//NewEntry.Instance->AddStatTagStack(TAG_Inventory_Item_Count, StackCount);

	MarkItemDirty(NewEntry);

	return Result;
}

void FHInventoryList::AddEntry(UHInventoryItemInstance* Instance)
{
	unimplemented();
}

void FHInventoryList::AddEntry(FHInventoryEntry& InEntry)
{
	Entries.Add(InEntry);
	MarkItemDirty(InEntry);
}

UHInventoryComponent::UHInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

bool UHInventoryComponent::CanAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount)
{
	//We can do logic here for item limits and such

	return true;
}

bool UHInventoryComponent::CanStackItemDefinition(UHItemDefinition* ItemDef, int32 StackCount)
{
	/*if(ItemDef->CanBeStacked)
	{
		for (FHInventoryEntry& Entry : InventoryArray.Entries)
		{
			InventoryGrid[0] = nullptr;
		}
	}*/

	return false;
}

UHInventoryItemInstance* UHInventoryComponent::AddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount)
{
	UHInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);

		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UHInventoryComponent::AddItemInstance(UHInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UHInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

void UHInventoryComponent::RemoveItemInstance(UHInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UHInventoryItemInstance*> UHInventoryComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UHInventoryItemInstance* UHInventoryComponent::FindFirstItemStackByDefinition(
	UHItemDefinition* ItemDef) const
{
	for (const FHInventoryEntry& Entry : InventoryList.Entries)
	{
		UHInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDefinition() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UHInventoryComponent::GetTotalItemCountByDefinition(UHItemDefinition* ItemDef) const
{
	int32 TotalCount = 0;
	for (const FHInventoryEntry& Entry : InventoryList.Entries)
	{
		UHInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDefinition() == ItemDef)
			{
				TotalCount++;
			}
		}
	}

	return TotalCount;
}

bool UHInventoryComponent::ConsumeItemsByDefinition(UHItemDefinition* ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UHInventoryItemInstance* Instance = UHInventoryComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			TotalConsumed++;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

bool UHInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FHInventoryEntry& Entry : InventoryList.Entries)
	{
		UHInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UHInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UHInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FHInventoryEntry& Entry : InventoryList.Entries)
		{
			UHInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}


