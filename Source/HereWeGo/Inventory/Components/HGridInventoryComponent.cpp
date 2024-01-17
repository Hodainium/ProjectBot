// Fill out your copyright notice in the Description page of Project Settings.


#include "HGridInventoryComponent.h"
#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Engine/ActorChannel.h"
#include "HereWeGo/Inventory/Grid/HInventoryGrid.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UHGridInventoryComponent::UHGridInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

// Called when the game starts
void UHGridInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UHGridInventoryComponent::SetInventorySize(int32 Width, int32 Height)
{
	InventorySize.X = Width;
	InventorySize.Y = Height;
}

void UHGridInventoryComponent::OnRep_InventorySize()
{
	UE_LOGFMT(LogHGame, Warning, "OnRep: Inventory Size has changed. Should have logic here");
}

bool UHGridInventoryComponent::TryAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount)
{
	

	
}

bool UHGridInventoryComponent::TryAddItemInstance(UHInventoryItemInstance* ItemInstance, int32 StackCount)
{
	//Here we need to somehow send itemDef to grid and have it check if there's room,
	//If so needs to return new Farray member or prediction
	AActor* OwningActor = GetOwner();
	check(OwningActor->HasAuthority());

	FHInventoryPoint Point = FHInventoryPoint();

	int32 RemainingStacks = StackCount;

	while(RemainingStacks > 0)
	{
		if (LocalGridArray->FindNextSlotPointForInstance(ItemInstance, Point))
		{
			////Add item to grid while making a predictive or fentry
			//LocalGridArray->TryAddItemAtPoint(Point, ItemInstance, Count);

			//Imagine this is TryAddItemToSlot(). We're trying to figure out where it should go
			UHGridItem* GridItemAtPoint = LocalGridArray->GetItemAtPoint(Point);

			if(GridItemAtPoint)
			{
				if(GridItemAtPoint->CanStackWith(ItemInstance))
				{
					RemainingStacks = GridItemAtPoint->TryToAddInstanceStack(ItemInstance, RemainingStacks);
					continue;
				}
			}

			if(UHGridItem* NewEntry = LocalGridArray->AddItemInstanceToGridAtPoint(Point, ItemInstance, RemainingStacks))
			{
				
			}
			

			/////////
		}
		else
		{
			break;
		}
	}

	
	
}

UHGridInventoryComponent::UHGridInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

bool UHGridInventoryComponent::CanAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount)
{
	//We can do logic here for item limits and such

	LocalGridArray.

	return true;
}

bool UHGridInventoryComponent::CanStackItemDefinition(UHItemDefinition* ItemDef, int32 StackCount)
{
	/*if(ItemDef->GetCanItemBeStacked)
	{
		for (FHInventoryEntry& Entry : InventoryList.Entries)
		{
			InventoryGrid[0] = nullptr;
		}
	}*/

	return false;
}

UHInventoryItemInstance* UHGridInventoryComponent::AddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount)
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

void UHGridInventoryComponent::AddItemInstance(UHInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UHGridInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

void UHGridInventoryComponent::RemoveItemInstance(UHInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UHInventoryItemInstance*> UHGridInventoryComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UHInventoryItemInstance* UHGridInventoryComponent::FindFirstItemStackByDefinition(
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

int32 UHGridInventoryComponent::GetTotalItemCountByDefinition(UHItemDefinition* ItemDef) const
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

bool UHGridInventoryComponent::ConsumeItemsByDefinition(UHItemDefinition* ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UHInventoryItemInstance* Instance = UHGridInventoryComponent::FindFirstItemStackByDefinition(ItemDef))
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

bool UHGridInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
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

void UHGridInventoryComponent::ReadyForReplication()
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

