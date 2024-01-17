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

void UHGridInventoryComponent::HandleMoveRequest()
{
	//Somehow get data in here that represents a desired move
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
			UHGridItem* GridItemAtPoint = LocalGridArray->GetItemAtPoint(Point);

			if(GridItemAtPoint)
			{
				if(GridItemAtPoint->CanStackWith(ItemInstance))
				{
					int32 StacksAdded = GridItemAtPoint->TryToAddInstanceStack(ItemInstance, RemainingStacks);

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

				UHGridItem* NewItem = NewObject<UHGridItem>();
				NewItem->LoadEntryData(NewInventoryEntry);

				LocalGridArray->AddItemToGrid(NewItem);

				RemainingStacks -= StacksToCreate;
			}
			return true;
		}
		else
		{
			break;
		}
	}

	
	
}

