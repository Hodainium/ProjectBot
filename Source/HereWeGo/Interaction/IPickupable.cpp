// Copyright Epic Games, Inc. All Rights Reserved.

#include "IPickupable.h"

#include "AsyncMixin.h"
#include "GameFramework/Actor.h"
#include "HInventoryComponent.h"
#include "HInventoryItemInstance.h"
#include "HItemDefinition.h"
#include "HItemSlotComponent.h"
#include "Engine/AssetManager.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IPickupable)

class UActorComponent;

UPickupableStatics::UPickupableStatics()
	: Super(FObjectInitializer::Get())
{
}

TScriptInterface<IPickupable> UPickupableStatics::GetFirstPickupableFromActor(AActor* Actor)
{
	// If the actor is directly pickupable, return that.
	TScriptInterface<IPickupable> PickupableActor(Actor);
	if (PickupableActor)
	{
		return PickupableActor;
	}

	// If the actor isn't pickupable, it might have a component that has a pickupable interface.
	TArray<UActorComponent*> PickupableComponents = Actor ? Actor->GetComponentsByInterface(UPickupable::StaticClass()) : TArray<UActorComponent*>();
	if (PickupableComponents.Num() > 0)
	{
		// Get first pickupable, if the user needs more sophisticated pickup distinction, will need to be solved elsewhere.
		return TScriptInterface<IPickupable>(PickupableComponents[0]);
	}

	return TScriptInterface<IPickupable>();
}

void UPickupableStatics::AddPickupToInventory(UHInventoryComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup)
{
	if (InventoryComponent && Pickup)
	{
		const FInventoryPickup& PickupInventory = Pickup->GetPickupInventory();

		for (const FPickupTemplate& Template : PickupInventory.Templates)
		{
			UHItemDefinition* ItemDefRef = Template.ItemDef.LoadSynchronous();
			InventoryComponent->AddItemDefinition(ItemDefRef, Template.StackCount);
		}

		for (const FPickupInstance& Instance : PickupInventory.Instances)
		{
			InventoryComponent->AddItemInstance(Instance.Item);
		}
	}
}

void UPickupableStatics::PushPickupToPlayer(APawn* PlayerPawn, TScriptInterface<IPickupable> Pickup)
{
	UHInventoryComponent* InventoryComponent = PlayerPawn->GetComponentByClass<UHInventoryComponent>();
	UHItemSlotComponent* SlotComponent = PlayerPawn->GetComponentByClass<UHItemSlotComponent>();
	
	if (InventoryComponent && SlotComponent && Pickup)
	{
		const FInventoryPickup& PickupInventory = Pickup->GetPickupInventory();

		for (const FPickupTemplate& Template : PickupInventory.Templates)
		{
			UHItemDefinition* ItemDefRef = Template.ItemDef.LoadSynchronous();
			UE_LOGFMT(LogHGame, Error, "WE ARE SYNC LOADING IN PUSHITEM TO PLAYER!!!!!!!!!!!!!!!!! FIX ASAP");

			//FAsyncMixin::AsyncLoad(Template.ItemDef, )
			//FStreamableManager::RequestAsyncLoad

			if(ItemDefRef->ItemType == EHItemType::Weapon)
			{
				UHInventoryItemInstance* ItemInstanceToAdd = InventoryComponent->AddItemDefinition(ItemDefRef, Template.StackCount);

				if(SlotComponent->GetSlotsForEnum(EHInventorySlotType::Temporary)[0] != nullptr)
				{
					UE_LOGFMT(LogHGame, Warning, "Dropping extra gun from pickup not yet implemented");
				}
				else
				{
					SlotComponent->AddItemToSlot(EHInventorySlotType::Temporary, 0, ItemInstanceToAdd);
				}
			}
			else
			{
				UE_LOGFMT(LogHGame, Error, "Dropping extra gun from pickup not yet implemented");
			}

			
			return;
		}

		for (const FPickupInstance& Instance : PickupInventory.Instances)
		{
			/*InventoryComponent->AddItemInstance(Instance.Item);
			SlotComponent->AddItemToSlot(EHInventorySlotType::Temporary, 0, Instance.Item);*/

			if (Instance.Item->GetItemTypeEnum() == EHItemType::Weapon)
			{
				InventoryComponent->AddItemInstance(Instance.Item);

				if (SlotComponent->GetSlotsForEnum(EHInventorySlotType::Temporary)[0] != nullptr)
				{
					UE_LOGFMT(LogHGame, Warning, "Dropping extra gun from pickup not yet implemented");
				}
				else
				{
					SlotComponent->AddItemToSlot(EHInventorySlotType::Temporary, 0, Instance.Item);
				}
			}

			return;
		}
	}
}

void UPickupableStatics::OnWeaponLoad()
{
}
