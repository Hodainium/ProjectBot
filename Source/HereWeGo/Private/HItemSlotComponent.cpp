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

UE_DEFINE_GAMEPLAY_TAG(TAG_QuickBar_Message_SlotsChanged, "Inventory.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_QuickBar_Message_ActiveIndexChanged, "Inventory.QuickBar.Message.ActiveIndexChanged");

UHItemSlotComponent::UHItemSlotComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UHItemSlotComponent::CycleActiveSlotForward(EHSlotType SlotType)
{
	TArray<UHInventoryItemInstance*> SlotArray;  
	int32 ActiveSlotIndex = INDEX_NONE;

	bool HasAllRequiredVars = GetSlotArrayForEnum(SlotType, SlotArray) && GetActiveSlotIndexForEnum(SlotType);

	if(!HasAllRequiredVars)
	{
		UE_LOGFMT(LogHGame, Error, "ItemSlot::CycleSlotForward: Um why are we changing the active slot on an item that does not have one.");
		return;
	}

	if (SlotArray.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? SlotArray.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % SlotArray.Num();
		if (SlotArray[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(SlotType, NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UHItemSlotComponent::CycleActiveSlotBackward(EHSlotType SlotType)
{
	TArray<UHInventoryItemInstance*> SlotArray;
	int32 ActiveSlotIndex = INDEX_NONE;

	bool HasAllRequiredVars = GetSlotArrayForEnum(SlotType, SlotArray) && GetActiveSlotIndexForEnum(SlotType);

	if (!HasAllRequiredVars)
	{
		UE_LOGFMT(LogHGame, Error, "ItemSlot::CycleActiveSlotBackward: Um why are we changing the active slot on an item that does not have one.");
		return;
	}

	if (SlotArray.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? SlotArray.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + SlotArray.Num()) % SlotArray.Num();
		if (SlotArray[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(SlotType, NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UHItemSlotComponent::SetActiveSlotIndex_Implementation(EHSlotType SlotType, int32 NewIndex)
{
	TArray<UHInventoryItemInstance*> SlotArray;
	SlotArray
	int32 ActiveSlotIndex = INDEX_NONE;

	bool HasAllRequiredVars = GetSlotArrayForEnum(SlotType, SlotArray) && GetActiveSlotIndexForEnum(SlotType);

	if (!HasAllRequiredVars)
	{
		UE_LOGFMT(LogHGame, Error, "ItemSlot::CycleActiveSlotBackward: Um why are we changing the active slot on an item that does not have one.");
		return;
	}

	if (SlotArray.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot(SlotType);

		ActiveSlotIndex = NewIndex;

		EquipItemInSlot(SlotType);

		Trigger_OnRep_ActiveSlotIndex_ForEnum(SlotType);
	}
}

UHInventoryItemInstance* UHItemSlotComponent::GetActiveSlotItem() const
{
	return WeaponSlots.IsValidIndex(ActiveSlotIndex) ? WeaponSlots[ActiveSlotIndex] : nullptr;
}

int32 UHItemSlotComponent::GetNextFreeItemSlot() const
{
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

void UHItemSlotComponent::AddItemToSlot(int32 SlotIndex, UHInventoryItemInstance* Item)
{
	if (WeaponSlots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (WeaponSlots[SlotIndex] == nullptr)
		{
			WeaponSlots[SlotIndex] = Item;
			OnRep_WeaponSlots();
		}
	}
}

UHInventoryItemInstance* UHItemSlotComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	UHInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot(TODO);
		ActiveSlotIndex = -1;
	}

	if (WeaponSlots.IsValidIndex(SlotIndex))
	{
		Result = WeaponSlots[SlotIndex];

		if (Result != nullptr)
		{
			WeaponSlots[SlotIndex] = nullptr;
			OnRep_WeaponSlots();
		}
	}

	return Result;
}


// Called when the game starts
void UHItemSlotComponent::BeginPlay()
{
	if (WeaponSlots.Num() < NumWeaponSlots)
	{
		WeaponSlots.AddDefaulted(NumWeaponSlots - WeaponSlots.Num());
	}

	Super::BeginPlay();
}

void UHItemSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WeaponSlots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

void UHItemSlotComponent::UnequipItemInSlot(EHSlotType SlotType)
{
	if (UHEquipmentComponent* EquipmentManager = FindEquipmentComponent())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
	TODO
}

void UHItemSlotComponent::EquipItemInSlot(EHSlotType SlotType)
{
	check(WeaponSlots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	if (UHInventoryItemInstance* SlotItem = WeaponSlots[ActiveSlotIndex])
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
				UE_LOG(LogHGame, Error, TEXT("ITEMSLOTCOMP::Bruh you didn't include equipItemInSlot logic. For item: %s"), *SlotItem->GetItemDefinition()->ItemName.ToString());
			}
		}

		TODO

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

void UHItemSlotComponent::OnRep_WeaponSlots()
{
	FHQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = WeaponSlots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_QuickBar_Message_SlotsChanged, Message);
}

void UHItemSlotComponent::OnRep_ActiveSlotIndex_Weapon_L()
{
	FHQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_QuickBar_Message_ActiveIndexChanged, Message);
}

bool UHItemSlotComponent::GetSlotArrayForEnum(EHSlotType SlotType, TArray<UHInventoryItemInstance*>& OutSlotArray) const
{
	OutSlotArray.Reset();

	switch (SlotType)
	{
	case EHSlotType::Weapon_L:
	{
		OutSlotArray = Slots_Weapon_L;
		break;
	}
	case EHSlotType::Weapon_R:
	{
		OutSlotArray = Slots_Weapon_R;
		break;
	}
	case EHSlotType::Armor_Head:
	{
		OutSlotArray = Slots_Armor_Head;
		break;
	}
	case EHSlotType::Armor_Chest:
	{
		OutSlotArray = Slots_Armor_Chest;
		break;
	}
	case EHSlotType::Armor_ArmL:
	{
		OutSlotArray = Slots_Armor_ArmL;
		break;
	}
	case EHSlotType::Armor_ArmR:
	{
		OutSlotArray = Slots_Armor_ArmR;
		break;
	}
	case EHSlotType::Armor_LegL:
	{
		OutSlotArray = Slots_Armor_LegL;
		break;
	}
	case EHSlotType::Armor_LegR:
	{
		OutSlotArray = Slots_Armor_LegR;
		break;
	}
	case EHSlotType::Armor_Core:
	{
		OutSlotArray = Slots_Armor_Core;
		break;
	}
	default:
	{
		return false;
	}
	}
	return true;
}

TArray<UHInventoryItemInstance*>& UHItemSlotComponent::GetSlotArrayRefForEnum(EHSlotType SlotType) const
{
	TArray<UHInventoryItemInstance*> Empty;
	return Empty;
}

int32 UHItemSlotComponent::GetNumSlotsForEnum(EHSlotType SlotType)
{
	switch (SlotType)
	{
		case EHSlotType::Weapon_L:
		{
			return NumSlots_Weapon_L;
		}
		case EHSlotType::Weapon_R:
		{
			return NumSlots_Weapon_R;
		}
		case EHSlotType::Armor_Head:
		{
			return NumSlots_Armor_Head;
		}
		case EHSlotType::Armor_Chest:
		{
			return NumSlots_Armor_Chest;
		}
		case EHSlotType::Armor_ArmL:
		{
			return NumSlots_Armor_ArmL;
		}
		case EHSlotType::Armor_ArmR:
		{
			return NumSlots_Armor_ArmR;
		}
		case EHSlotType::Armor_LegL:
		{
			return NumSlots_Armor_LegL;
		}
		case EHSlotType::Armor_LegR:
		{
			return NumSlots_Armor_LegR;
		}
		case EHSlotType::Armor_Core:
		{
			return NumSlots_Armor_Core;
		}
		default:
		{
			return INDEX_NONE;
		}
	}
}

void UHItemSlotComponent::SetNumSlotsForEnum(EHSlotType SlotType, int32 NewNumSlots)
{
	switch (SlotType)
	{
	case EHSlotType::Weapon_L:
	{
		NumSlots_Weapon_L = NewNumSlots;
	}
	case EHSlotType::Weapon_R:
	{
		NumSlots_Weapon_R = NewNumSlots;
	}
	case EHSlotType::Armor_Head:
	{
		NumSlots_Armor_Head = NewNumSlots;
	}
	case EHSlotType::Armor_Chest:
	{
		NumSlots_Armor_Chest = NewNumSlots;
	}
	case EHSlotType::Armor_ArmL:
	{
		NumSlots_Armor_ArmL = NewNumSlots;
	}
	case EHSlotType::Armor_ArmR:
	{
		NumSlots_Armor_ArmR = NewNumSlots;
	}
	case EHSlotType::Armor_LegL:
	{
		NumSlots_Armor_LegL = NewNumSlots;
	}
	case EHSlotType::Armor_LegR:
	{
		NumSlots_Armor_LegR = NewNumSlots;
	}
	case EHSlotType::Armor_Core:
	{
		NumSlots_Armor_Core = NewNumSlots;
	}
	default:
	{
		return;
	}
	}
}

int32 UHItemSlotComponent::GetActiveSlotIndexForEnum(EHSlotType SlotType) const
{
	switch (SlotType)
	{
		case EHSlotType::Weapon_L:
		{
			return ActiveSlotIndex_Weapon_L;
		}
		case EHSlotType::Weapon_R:
		{
			return ActiveSlotIndex_Weapon_R;
		}
		case EHSlotType::Armor_Head:
		{
			return ActiveSlotIndex_Armor_Head;
		}
		case EHSlotType::Armor_Chest:
		{
			return ActiveSlotIndex_Armor_Chest;
		}
		case EHSlotType::Armor_ArmL:
		{
			return ActiveSlotIndex_Armor_ArmL;
		}
		case EHSlotType::Armor_ArmR:
		{
			return ActiveSlotIndex_Armor_ArmR;
		}
		case EHSlotType::Armor_LegL:
		{
			return ActiveSlotIndex_Armor_LegL;
		}
		case EHSlotType::Armor_LegR:
		{
			return ActiveSlotIndex_Armor_LegR;
		}
		case EHSlotType::Armor_Core:
		{
			return ActiveSlotIndex_Armor_Core;
		}
		default:
		{
			return INDEX_NONE;
		}
	}
}

bool UHItemSlotComponent::SetActiveSlotIndexForEnum(EHSlotType SlotType, int32 NewActiveSlotIndex)
{
	switch (SlotType)
	{
	case EHSlotType::Weapon_L:
	{
		ActiveSlotIndex_Weapon_L = NewActiveSlotIndex;
		break;
	}
	case EHSlotType::Weapon_R:
	{
		ActiveSlotIndex_Weapon_R = NewActiveSlotIndex;
		break;
	}
	case EHSlotType::Armor_Head:
	{
		ActiveSlotIndex_Armor_Head = NewActiveSlotIndex;
		break;
	}
	case EHSlotType::Armor_Chest:
	{
		ActiveSlotIndex_Armor_Chest = NewActiveSlotIndex;
		break;
	}
	case EHSlotType::Armor_ArmL:
	{
		ActiveSlotIndex_Armor_ArmL = NewActiveSlotIndex;
		break;
	}
	case EHSlotType::Armor_ArmR:
	{
		ActiveSlotIndex_Armor_ArmR = NewActiveSlotIndex;
		break;
	}
	case EHSlotType::Armor_LegL:
	{
		ActiveSlotIndex_Armor_LegL = NewActiveSlotIndex;
		break;
	}
	case EHSlotType::Armor_LegR:
	{
		ActiveSlotIndex_Armor_LegR = NewActiveSlotIndex;
		break;
	}
	case EHSlotType::Armor_Core:
	{
		ActiveSlotIndex_Armor_Core = NewActiveSlotIndex;
		break;
	}
	default:
	{
		return false;
	}
	}
	return true;
}

bool UHItemSlotComponent::Trigger_OnRep_Slots_ForEnum(EHSlotType SlotType)
{
	switch (SlotType)
	{
		case EHSlotType::Weapon_L:
		{
			OnRep_Slots_Weapon_L();
			break;
		}
		case EHSlotType::Weapon_R:
		{
			OnRep_Slots_Weapon_R();
			break;
		}
		case EHSlotType::Armor_Head:
		{
			OnRep_Slots_Armor_Head();
			break;
		}
		case EHSlotType::Armor_Chest:
		{
			OnRep_Slots_Armor_Chest();
			break;
		}
		case EHSlotType::Armor_ArmL:
		{
			OnRep_Slots_Armor_ArmL();
			break;
		}
		case EHSlotType::Armor_ArmR:
		{
			OnRep_Slots_Armor_ArmR();
			break;
		}
		case EHSlotType::Armor_LegL:
		{
			OnRep_Slots_Armor_LegL();
			break;
		}
		case EHSlotType::Armor_LegR:
		{
			OnRep_Slots_Armor_LegR();
			break;
		}
		case EHSlotType::Armor_Core:
		{
			OnRep_Slots_Armor_Core();
			break;
		}
		default:
		{
			return false;
		}
	}

	return true;
}

bool UHItemSlotComponent::Trigger_OnRep_NumSlots_ForEnum(EHSlotType SlotType)
{
	switch (SlotType)
	{
		case EHSlotType::Weapon_L:
		{
			OnRep_NumSlots_Weapon_L();
			break;
		}
		case EHSlotType::Weapon_R:
		{
			OnRep_NumSlots_Weapon_R();
			break;
		}
		case EHSlotType::Armor_Head:
		{
			OnRep_NumSlots_Armor_Head();
			break;
		}
		case EHSlotType::Armor_Chest:
		{
			OnRep_NumSlots_Armor_Chest();
			break;
		}
		case EHSlotType::Armor_ArmL:
		{
			OnRep_NumSlots_Armor_ArmL();
			break;
		}
		case EHSlotType::Armor_ArmR:
		{
			OnRep_NumSlots_Armor_ArmR();
			break;
		}
		case EHSlotType::Armor_LegL:
		{
			OnRep_NumSlots_Armor_LegL();
			break;
		}
		case EHSlotType::Armor_LegR:
		{
			OnRep_NumSlots_Armor_LegR();
			break;
		}
		case EHSlotType::Armor_Core:
		{
			OnRep_NumSlots_Armor_Core();
			break;
		}
		default:
		{
			return false;
		}
	}

	return true;
}

bool UHItemSlotComponent::Trigger_OnRep_ActiveSlotIndex_ForEnum(EHSlotType SlotType)
{
	switch (SlotType)
	{
		case EHSlotType::Weapon_L:
		{
			OnRep_ActiveSlotIndex_Weapon_L();
			break;
		}
		case EHSlotType::Weapon_R:
		{
			OnRep_ActiveSlotIndex_Weapon_R();
			break;
		}
		case EHSlotType::Armor_Head:
		{
			OnRep_ActiveSlotIndex_Armor_Head();
			break;
		}
		case EHSlotType::Armor_Chest:
		{
			OnRep_ActiveSlotIndex_Armor_Chest();
			break;
		}
		case EHSlotType::Armor_ArmL:
		{
			OnRep_ActiveSlotIndex_Armor_ArmL();
			break;
		}
		case EHSlotType::Armor_ArmR:
		{
			OnRep_ActiveSlotIndex_Armor_ArmR();
			break;
		}
		case EHSlotType::Armor_LegL:
		{
			OnRep_ActiveSlotIndex_Armor_LegL();
			break;
		}
		case EHSlotType::Armor_LegR:
		{
			OnRep_ActiveSlotIndex_Armor_LegR();
			break;
		}
		case EHSlotType::Armor_Core:
		{
			OnRep_ActiveSlotIndex_Armor_Core();
			break;
		}
		default:
		{
			return false;
		}
	}

	return true;
}

