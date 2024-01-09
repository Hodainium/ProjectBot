// Fill out your copyright notice in the Description page of Project Settings.


#include "HItemSlotComponent.h"

#include "HEquipmentComponent.h"
#include "HEquipmentDefinition.h"
#include "HEquipmentInstance.h"
#include "HInventoryFragmentTypes.h"
#include "HInventoryInterface.h"
#include "HItemDefinition.h"
#include "NativeGameplayTags.h"
#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "HereWeGo/HAssetManager.h"
#include "HereWeGo/Items/HWeaponItemDefinition.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_QuickBar_Message_SlotsChanged, "Inventory.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_QuickBar_Message_ActiveIndexChanged, "Inventory.QuickBar.Message.ActiveIndexChanged");

UHItemSlotComponent::UHItemSlotComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UHItemSlotComponent::CycleSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlot(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UHItemSlotComponent::CycleSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlot(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UHItemSlotComponent::SetActiveSlot_Implementation(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();

		ActiveSlotIndex = NewIndex;

		EquipItemInSlot();

		OnRep_ActiveSlotIndex();
	}
}

UHInventoryItemInstance* UHItemSlotComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 UHItemSlotComponent::GetNextFreeItemSlot() const
{
	int32 SlotIndex = 0;
	for (TObjectPtr<UHInventoryItemInstance> ItemPtr : Slots)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void UHItemSlotComponent::AddItemToSlot(int32 SlotIndex, UHInventoryItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
			OnRep_Slots();
		}
	}
}

UHInventoryItemInstance* UHItemSlotComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	UHInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			OnRep_Slots();
		}
	}

	return Result;
}


// Called when the game starts
void UHItemSlotComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void UHItemSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

void UHItemSlotComponent::UnequipItemInSlot()
{
	if (UHEquipmentComponent* EquipmentManager = FindEquipmentComponent())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

void UHItemSlotComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	if (UHInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		EHItemType DefItemType = SlotItem->GetItemDefinition()->ItemType;

		switch(DefItemType)
		{
			case EHItemType::Weapon:
			{
				if (UHWeaponItemDefinition* ItemDef = Cast<UHWeaponItemDefinition>(SlotItem->GetItemDefinition()))
				{
					if (UHEquipmentComponent* EquipmentManager = FindEquipmentComponent())
					{
						EquippedItem = EquipmentManager->EquipItem(ItemDef->WeaponDefinition);

						if (EquippedItem != nullptr)
						{
							EquippedItem->SetInstigator(SlotItem);
						}
					}
				}
			}
			default:
			{
				UE_LOG(LogHGame, Error, TEXT("ITEMSLOTCOMP::Bruh you didn't include equipItemInSlot logic. For item: %s"), *SlotItem->GetName());
			}
		}

		/*if (const UHInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UHInventoryFragment_EquippableItem>())
		{
			if (EquipInfo->EquipmentDefinition != nullptr)
			{
				if (UHEquipmentComponent* EquipmentManager = FindEquipmentComponent())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipInfo->EquipmentDefinition);
					if (EquippedItem != nullptr)
					{
						EquippedItem->SetInstigator(SlotItem);
					}
				}
			}
		}*/
	}
}

UHEquipmentComponent* UHItemSlotComponent::FindEquipmentComponent() const
{
	if (const IHInventoryInterface* InventoryInterface = Cast<IHInventoryInterface>(GetOwner()))
	{
		return InventoryInterface->GetEquipmentComponent();
	}
	return nullptr;
}

void UHItemSlotComponent::OnRep_Slots()
{
	FHQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_QuickBar_Message_SlotsChanged, Message);
}

void UHItemSlotComponent::OnRep_ActiveSlotIndex()
{
	FHQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_QuickBar_Message_ActiveIndexChanged, Message);
}

