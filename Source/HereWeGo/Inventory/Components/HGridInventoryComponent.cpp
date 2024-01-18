// Fill out your copyright notice in the Description page of Project Settings.


#include "HGridInventoryComponent.h"
#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Engine/ActorChannel.h"
#include "HereWeGo/Inventory/Grid/HInventoryGrid.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

UHGridInventoryComponent::UHGridInventoryComponent(const FObjectInitializer& ObjectInitializer)
{
}

bool UHGridInventoryComponent::CanStackEntries(UHGridEntry* BaseItem, UHGridEntry* RequestingItem)
{
	return BaseItem->CanStackWith(RequestingItem);
}

int32 UHGridInventoryComponent::StackEntries(UHGridEntry* BaseItem, UHGridEntry* RequestingItem)
{
	return LocalGridArray->TryToStackEntries(BaseItem, RequestingItem);
}

bool UHGridInventoryComponent::GetAllBlockingEntriesAtPointForEntry(FHInventoryPoint Point, UHGridEntry* RequestingEntry,
                                                                    TArray<UHGridEntry*> OutBlockingEntries)
{
	OutBlockingEntries.Reset();

	return LocalGridArray->GetAllBlockingItemsAtPoint(Point, RequestingEntry, OutBlockingEntries);
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
	UE_LOGFMT(LogHGame, Error, "Cannot add itemDef. This is unimplemented for now TryAddItemDefinition inv comp");
	return false;
}

int32 UHGridInventoryComponent::TryAddItemInstance(UHInventoryItemInstance* ItemInstance, int32 StackCount)
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
			UHGridEntry* GridItemAtPoint = LocalGridArray->GetItemAtPoint(Point);

			if(GridItemAtPoint)
			{
				if(GridItemAtPoint->CanStackWith(ItemInstance))
				{
					int32 StacksAdded = LocalGridArray->TryToAddInstanceStackToEntry(GridItemAtPoint, ItemInstance, RemainingStacks);

					if(StacksAdded > 0)
					{
						if(InventoryList.MarkItemIDDirty(GridItemAtPoint->LinkedRepID))
						{
							RemainingStacks -= StacksAdded;
						}
					}
				}
			}
			else
			{
				int32 MaxStackCanCreate = ItemInstance->GetMaxStack();
				int32 StacksToCreate = RemainingStacks;

				if(MaxStackCanCreate < RemainingStacks)
				{
					StacksToCreate = MaxStackCanCreate;
				}

				FHInventoryEntry NewInventoryEntry = FHInventoryEntry(ItemInstance, Point, false, StacksToCreate);
				InventoryList.AddEntry(NewInventoryEntry);

				UHGridEntry* NewItem = NewObject<UHGridEntry>();
				NewItem->LoadEntryData(NewInventoryEntry);

				LocalGridArray->AddItemToGrid(NewItem);

				RemainingStacks -= StacksToCreate;
			}
		}
		else //If we can't find another best point return remaining entries
		{
			return RemainingStacks;
		}
	}
	return 0;
}

