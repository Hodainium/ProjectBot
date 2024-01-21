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
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_ItemSlots_Message_SlotsChanged, "Inventory.ItemSlots.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_ItemSlots_Message_ActiveIndexChanged, "Inventory.ItemSlots.Message.ActiveIndexChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_ItemSlots_Message_NumSlotsChanged, "Inventory.ItemSlots.Message.ActiveIndexChanged");

UHItemSlotComponent::UHItemSlotComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UHItemSlotComponent::CycleActiveSlotForward(EHWeaponSlotType SlotType)
{
	const TArray<UHInventoryItemInstance*>& SlotArrayRef = GetSlotArrayForItemSlotEnum_NonMutable(SlotType);
	const int32 ActiveSlotIndexRef = GetActiveSlotIndexForItemSlotEnum_NonMutable(SlotType);

	if (SlotArrayRef.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndexRef < 0 ? SlotArrayRef.Num() - 1 : ActiveSlotIndexRef);
	int32 NewIndex = ActiveSlotIndexRef;
	do
	{
		if (SlotArrayRef[NewIndex] != nullptr)
		{
			NewIndex = (NewIndex + 1) % SlotArrayRef.Num();
			SetActiveSlotIndexForEnum(SlotType, NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UHItemSlotComponent::CycleActiveSlotBackward(EHWeaponSlotType SlotType)
{
	const TArray<UHInventoryItemInstance*>& SlotArrayRef = GetSlotArrayForItemSlotEnum_NonMutable(SlotType);
	const int32 ActiveSlotIndexRef = GetActiveSlotIndexForItemSlotEnum_NonMutable(SlotType);

	if (SlotArrayRef.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndexRef < 0 ? SlotArrayRef.Num() - 1 : ActiveSlotIndexRef);
	int32 NewIndex = ActiveSlotIndexRef;
	do
	{
		NewIndex = (NewIndex - 1 + SlotArrayRef.Num()) % SlotArrayRef.Num();
		if (SlotArrayRef[NewIndex] != nullptr)
		{
			SetActiveSlotIndexForEnum(SlotType, NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UHItemSlotComponent::SetActiveSlotIndexForEnum_Implementation(EHWeaponSlotType SlotType, int32 NewIndex)
{
	const TArray<UHInventoryItemInstance*>& SlotArrayRef = GetSlotArrayForItemSlotEnum_NonMutable(SlotType);
	int32 ActiveSlotIndexRef = GetActiveSlotIndexForItemSlotEnum_Mutable(SlotType);

	if (SlotArrayRef.IsValidIndex(NewIndex) && (ActiveSlotIndexRef != NewIndex))
	{
		UnequipItemInSlot(SlotType);

		ActiveSlotIndexRef = NewIndex;

		EquipItemInSlot(SlotType);
		Trigger_OnRep_ActiveSlotIndex_ForEnum(SlotType);
	}
}

UHInventoryItemInstance* UHItemSlotComponent::GetActiveSlotItem(EHWeaponSlotType SlotType) const
{
	const TArray<UHInventoryItemInstance*>& WeaponSlots = GetSlotArrayForItemSlotEnum_NonMutable(SlotType);
	const int32 ActiveSlotIndex = GetActiveSlotIndexForItemSlotEnum_NonMutable(SlotType);

	return WeaponSlots.IsValidIndex(ActiveSlotIndex) ? WeaponSlots[ActiveSlotIndex] : nullptr;
}

int32 UHItemSlotComponent::GetNextFreeItemSlot(EHWeaponSlotType SlotType) const
{
	const TArray<UHInventoryItemInstance*>& WeaponSlots = GetSlotArrayForItemSlotEnum_NonMutable(SlotType);
	int32 SlotIndex = 0;
	for (TObjectPtr<UHInventoryItemInstance> ItemPtr : WeaponSlots)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void UHItemSlotComponent::AddItemToSlot(EHWeaponSlotType SlotType, int32 SlotIndex, UHInventoryItemInstance* Item)
{
	TArray<UHInventoryItemInstance*> WeaponSlots = GetSlotArrayForItemSlotEnum_Mutable(SlotType);

	if (WeaponSlots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (WeaponSlots[SlotIndex] == nullptr)
		{
			WeaponSlots[SlotIndex] = Item;
			Trigger_OnRep_Slots_ForEnum(SlotType);
		}
	}
}

UHInventoryItemInstance* UHItemSlotComponent::RemoveItemFromSlot(EHWeaponSlotType SlotType, int32 SlotIndex)
{
	TArray<UHInventoryItemInstance*> WeaponSlots = GetSlotArrayForItemSlotEnum_Mutable(SlotType);
	int32 ActiveSlotIndex = GetActiveSlotIndexForItemSlotEnum_Mutable(SlotType);
	UHInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot(SlotType);
		ActiveSlotIndex = -1;
	}

	if (WeaponSlots.IsValidIndex(SlotIndex))
	{
		Result = WeaponSlots[SlotIndex];

		if (Result != nullptr)
		{
			WeaponSlots[SlotIndex] = nullptr;
			Trigger_OnRep_Slots_ForEnum(SlotType);
		}
	}

	return Result;
}


// Called when the game starts
void UHItemSlotComponent::BeginPlay()
{
	HandleResizeSlotArrayForEnum(EHWeaponSlotType::Weapon_L);
	HandleResizeSlotArrayForEnum(EHWeaponSlotType::Weapon_R);

	for (EHWeaponSlotType SlotType : TEnumRange<EHWeaponSlotType>())
	{
		switch(SlotType)
		{
		case EHWeaponSlotType::Weapon_L:
			{
				SlotsDataMap.Add(FHSlotDataMap(&Slots_Weapon_L, &NumSlots_Weapon_L, &ActiveSlotIndex_Weapon_L, &EquippedItem_Weapon_L));
			}
		case EHWeaponSlotType::Weapon_R:
			{
				SlotsDataMap.Add(FHSlotDataMap(&Slots_Weapon_R, &NumSlots_Weapon_R, &ActiveSlotIndex_Weapon_R, &EquippedItem_Weapon_R));
			}
			default:
			{
				break;
			}
		}
	}

	Super::BeginPlay();
}

void UHItemSlotComponent::HandleResizeSlotArrayForEnum(EHWeaponSlotType SlotType)
{
	TArray<UHInventoryItemInstance*> WeaponSlots = GetSlotArrayForItemSlotEnum_Mutable(SlotType);
	const int32 NumWeaponSlots = GetNumSlotsForItemSlotEnum_NonMutable(SlotType);

	if (WeaponSlots.Num() < NumWeaponSlots)
	{
		WeaponSlots.AddDefaulted(NumWeaponSlots - WeaponSlots.Num());
	}
	else
	{
		UE_LOGFMT(LogHGame, Error, "Downsizing array not yet implemented");
	}
}

void UHItemSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots_Weapon_L);
	DOREPLIFETIME(ThisClass, Slots_Weapon_L);
	DOREPLIFETIME(ThisClass, NumSlots_Weapon_L);
	DOREPLIFETIME(ThisClass, NumSlots_Weapon_R);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex_Weapon_L);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex_Weapon_R);
}

void UHItemSlotComponent::UnequipItemInSlot(EHWeaponSlotType SlotType)
{
	if (UHEquipmentComponent* EquipmentManager = FindEquipmentComponent())
	{
		UHEquipmentInstance* EquippedItem = GetEquippedItem(SlotType);
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

void UHItemSlotComponent::EquipItemInSlot(EHWeaponSlotType SlotType)
{
	const TArray<UHInventoryItemInstance*>& WeaponSlots = GetSlotArrayForItemSlotEnum_NonMutable(SlotType);
	const int32 ActiveSlotIndex = GetActiveSlotIndexForItemSlotEnum_NonMutable(SlotType);
	UHEquipmentInstance* EquippedItem = GetEquippedItem(SlotType);

	check(WeaponSlots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	if (UHInventoryItemInstance* SlotItem = WeaponSlots[ActiveSlotIndex])
	{
		EHItemType DefItemType = SlotItem->GetItemDefinition()->ItemType;

		switch (DefItemType)
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
			UE_LOG(LogHGame, Error, TEXT("ITEMSLOTCOMP::Bruh you didn't include equipItemInSlot logic. For item: %s"), *SlotItem->GetItemDefinition()->ItemName.ToString());
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

void UHItemSlotComponent::BroadcastSlotsChanged(EHWeaponSlotType SlotType)
{
	FHItemSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = GetSlotArrayForItemSlotEnum_NonMutable(SlotType);
	Message.SlotType = SlotType;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_ItemSlots_Message_SlotsChanged, Message);
}

void UHItemSlotComponent::BroadcastNumSlotsChanged(EHWeaponSlotType SlotType)
{
	FHItemSlotsNumSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.NumSlots = GetNumSlotsForItemSlotEnum_NonMutable(SlotType);
	Message.SlotType = SlotType;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_ItemSlots_Message_ActiveIndexChanged, Message);
}

void UHItemSlotComponent::BroadcastActiveSlotIndexChanged(EHWeaponSlotType SlotType) const
{
	FHItemSlotsActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = GetActiveSlotIndexForItemSlotEnum_NonMutable(SlotType);
	Message.SlotType = SlotType;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_ItemSlots_Message_NumSlotsChanged, Message);
}

void UHItemSlotComponent::OnRep_Slots_Weapon_L()
{
	BroadcastSlotsChanged(EHWeaponSlotType::Weapon_L);
}

void UHItemSlotComponent::OnRep_NumSlots_Weapon_L()
{
	BroadcastNumSlotsChanged(EHWeaponSlotType::Weapon_L);
}

void UHItemSlotComponent::OnRep_ActiveSlotIndex_Weapon_L()
{
	BroadcastActiveSlotIndexChanged(EHWeaponSlotType::Weapon_L);
}

void UHItemSlotComponent::OnRep_Slots_Weapon_R()
{
	BroadcastSlotsChanged(EHWeaponSlotType::Weapon_R);
}

void UHItemSlotComponent::OnRep_NumSlots_Weapon_R()
{
	BroadcastNumSlotsChanged(EHWeaponSlotType::Weapon_R);
}

void UHItemSlotComponent::OnRep_ActiveSlotIndex_Weapon_R()
{
	BroadcastActiveSlotIndexChanged(EHWeaponSlotType::Weapon_R);
}

TArray<UHInventoryItemInstance*> UHItemSlotComponent::GetSlotArrayForItemSlotEnum_Mutable(EHWeaponSlotType SlotType)
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
	{
		return Slots_Weapon_L;
	}
	case EHWeaponSlotType::Weapon_R:
	{
		return Slots_Weapon_R;
	}
	default:
	{
		UE_LOGFMT(LogHGame, Error, "Could not access SlotsArray for enum in itemslotComp this is super bad");
		TArray<UHInventoryItemInstance*> DefaultReturn;
		return DefaultReturn;
	}
	}
}

const TArray<UHInventoryItemInstance*> UHItemSlotComponent::GetSlotArrayForItemSlotEnum_NonMutable(
	EHWeaponSlotType SlotType) const
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
	{
		return Slots_Weapon_L;
	}
	case EHWeaponSlotType::Weapon_R:
	{
		return Slots_Weapon_R;
	}
	default:
	{
		UE_LOGFMT(LogHGame, Error, "Could not access SlotsArray for enum in itemslotComp this is super bad");
		TArray<UHInventoryItemInstance*> DefaultReturn;
		return DefaultReturn;
	}
	}
}

int32 UHItemSlotComponent::GetNumSlotsForItemSlotEnum_Mutable(EHWeaponSlotType SlotType)
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
		return NumSlots_Weapon_L;
	case EHWeaponSlotType::Weapon_R:
		return NumSlots_Weapon_R;
	default:
		return NumSlots_Weapon_L;
	}
}

int32 UHItemSlotComponent::GetNumSlotsForItemSlotEnum_NonMutable(EHWeaponSlotType SlotType) const
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
		return NumSlots_Weapon_L;
	case EHWeaponSlotType::Weapon_R:
		return NumSlots_Weapon_R;
	default:
		return NumSlots_Weapon_L;
	}
}

int32 UHItemSlotComponent::GetActiveSlotIndexForItemSlotEnum_Mutable(EHWeaponSlotType SlotType)
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
		return ActiveSlotIndex_Weapon_L;
	case EHWeaponSlotType::Weapon_R:
		return ActiveSlotIndex_Weapon_R;
	default:
		return ActiveSlotIndex_Weapon_L;
	}
}

int32 UHItemSlotComponent::GetActiveSlotIndexForItemSlotEnum_NonMutable(EHWeaponSlotType SlotType) const
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
		return ActiveSlotIndex_Weapon_L;
	case EHWeaponSlotType::Weapon_R:
		return ActiveSlotIndex_Weapon_R;
	default:
		return ActiveSlotIndex_Weapon_L;
	}
}

UHEquipmentInstance* UHItemSlotComponent::GetEquippedItem(EHWeaponSlotType SlotType)
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
	{
		return EquippedItem_Weapon_L;
	}
	case EHWeaponSlotType::Weapon_R:
	{
		return EquippedItem_Weapon_R;
	}
	default:
	{
		UE_LOGFMT(LogHGame, Error, "Could not access EquippedItem for enum in itemslotComp this is super bad");
		return nullptr;
	}
	}
}

bool UHItemSlotComponent::Trigger_OnRep_Slots_ForEnum(EHWeaponSlotType SlotType)
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
	{
		OnRep_Slots_Weapon_L();
		break;
	}
	case EHWeaponSlotType::Weapon_R:
	{
		OnRep_Slots_Weapon_R();
		break;
	}
	default:
	{
		return false;
	}
	}

	return true;
}

bool UHItemSlotComponent::Trigger_OnRep_NumSlots_ForEnum(EHWeaponSlotType SlotType)
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
	{
		OnRep_NumSlots_Weapon_L();
		break;
	}
	case EHWeaponSlotType::Weapon_R:
	{
		OnRep_NumSlots_Weapon_R();
		break;
	}
	default:
	{
		return false;
	}
	}

	return true;
}

bool UHItemSlotComponent::Trigger_OnRep_ActiveSlotIndex_ForEnum(EHWeaponSlotType SlotType)
{
	switch (SlotType)
	{
	case EHWeaponSlotType::Weapon_L:
	{
		OnRep_ActiveSlotIndex_Weapon_L();
		break;
	}
	case EHWeaponSlotType::Weapon_R:
	{
		OnRep_ActiveSlotIndex_Weapon_R();
		break;
	}
	default:
	{
		return false;
	}
	}

	return true;
}

