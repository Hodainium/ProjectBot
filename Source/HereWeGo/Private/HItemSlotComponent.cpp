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
#include "HereWeGo/Tags/H_Tags.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

UHItemSlotComponent::UHItemSlotComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UHItemSlotComponent::BeginPlay()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		SetNumSlotsForEnum(EHInventorySlotType::Weapon_L, WeaponLStartingSlots);
		SetNumSlotsForEnum(EHInventorySlotType::Weapon_R, WeaponRStartingSlots);
		SetNumSlotsForEnum(EHInventorySlotType::Temporary, TemporaryStartingSlots);
	}
}

void UHItemSlotComponent::CycleActiveSlotForward(EHInventorySlotType SlotType)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);

	if (Slots.SlotArray.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (Slots.ActiveSlotIndex < 0 ? Slots.SlotArray.Num() - 1 : Slots.ActiveSlotIndex);
	int32 NewIndex = Slots.ActiveSlotIndex;
	do
	{
		if (Slots.SlotArray[NewIndex] != nullptr)
		{
			NewIndex = (NewIndex + 1) % Slots.SlotArray.Num();
			SetActiveSlotIndexForEnum(SlotType, NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UHItemSlotComponent::CycleActiveSlotBackward(EHInventorySlotType SlotType)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);

	if (Slots.SlotArray.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (Slots.ActiveSlotIndex < 0 ? Slots.SlotArray.Num() - 1 : Slots.ActiveSlotIndex);
	int32 NewIndex = Slots.ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.SlotArray.Num()) % Slots.SlotArray.Num();
		if (Slots.SlotArray[NewIndex] != nullptr)
		{
			SetActiveSlotIndexForEnum(SlotType, NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UHItemSlotComponent::SetActiveSlotIndexForEnum_Implementation(EHInventorySlotType SlotType, int32 NewIndex)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);


	if (Slots.SlotArray.IsValidIndex(NewIndex) && (Slots.ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot(SlotType);

		Slots.ActiveSlotIndex = NewIndex;

		EquipItemInSlot(SlotType);
		Handle_OnRep_ActiveSlotIndexChanged(SlotType);
	}
}

UHInventoryItemInstance* UHItemSlotComponent::GetActiveSlotItem(EHInventorySlotType SlotType) const
{
	const FHInventorySlotStruct& Slots = GetSlotStructForEnum_Const(SlotType);

	return Slots.SlotArray.IsValidIndex(Slots.ActiveSlotIndex) ? Slots.SlotArray[Slots.ActiveSlotIndex] : nullptr;
}

int32 UHItemSlotComponent::GetNextFreeItemSlot(EHInventorySlotType SlotType) const
{
	const FHInventorySlotStruct& Slots = GetSlotStructForEnum_Const(SlotType);

	int32 SlotIndex = 0;
	for (TObjectPtr<UHInventoryItemInstance> ItemPtr : Slots.SlotArray)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void UHItemSlotComponent::SetNumSlotsForEnum(EHInventorySlotType SlotType, int32 InNum)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);


	UE_LOGFMT(LogHGame, Warning, "Set num slots 1");

	if(Slots.SlotArray.Num() == InNum)
	{
		return;
	}

	UE_LOGFMT(LogHGame, Warning, "Set num slots 2");


	if (Slots.SlotArray.Num() < InNum)
	{
		Slots.SlotArray.AddDefaulted(Slots.NumSlots - Slots.SlotArray.Num());
	}
	else if (Slots.SlotArray.Num() > InNum)
	{
		//Downsizing array not yet implemented
		UE_LOGFMT(LogHGame, Warning, "Removing items but not yet dropping. Need to implement dropping items");

		for (int i = 0; i < (InNum - Slots.SlotArray.Num()); i++)
		{
			//Should drop items at these indexes
			Slots.SlotArray[Slots.SlotArray.Num() - i] = nullptr;
			UE_LOGFMT(LogHGame, Warning, "Removing item at index: {idx}", i);
		}
	}
	
	Handle_OnRep_NumSlotsChanged(SlotType);
}

void UHItemSlotComponent::AddItemToSlot(EHInventorySlotType SlotType, int32 SlotIndex, UHInventoryItemInstance* Item)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);

	UE_LOGFMT(LogHGame, Warning, "At least this is happening");

	UE_LOGFMT(LogHGame, Warning, "size {size}", GetNumSlotsForEnum(EHInventorySlotType::Temporary));
	UE_LOGFMT(LogHGame, Warning, "direct size {size}", Slots.SlotArray.Num());
	UE_LOGFMT(LogHGame, Warning, "{idx}", SlotIndex);

	if (Slots.SlotArray.IsValidIndex(SlotIndex))
	{
		UE_LOGFMT(LogHGame, Warning, "At least this is happening1");
	}

	if(Item != nullptr)
	{
		UE_LOGFMT(LogHGame, Warning, "At least this is happening2");

	}

	if (Slots.SlotArray.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots.SlotArray[SlotIndex] == nullptr)
		{
			UE_LOGFMT(LogHGame, Warning, "Changing slot at index: {idx}", SlotIndex);
			Slots.SlotArray[SlotIndex] = Item;
			Handle_OnRep_SlotsChanged(SlotType);
		}
	}
}

UHInventoryItemInstance* UHItemSlotComponent::RemoveItemFromSlot(EHInventorySlotType SlotType, int32 SlotIndex)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);

	UHInventoryItemInstance* Result = nullptr;

	if (Slots.ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot(SlotType);
		Slots.ActiveSlotIndex = -1;
	}

	if (Slots.SlotArray.IsValidIndex(SlotIndex))
	{
		Result = Slots.SlotArray[SlotIndex];

		if (Result != nullptr)
		{
			Slots.SlotArray[SlotIndex] = nullptr;
			Handle_OnRep_SlotsChanged(SlotType);
		}
	}

	return Result;
}

void UHItemSlotComponent::HandleResizeSlotArrayForEnum(EHInventorySlotType SlotType)
{
	//FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);

	//if (Slots.SlotArray.Num() < Slots.NumSlots)
	//{
	//	Slots.SlotArray.AddDefaulted(Slots.NumSlots - Slots.SlotArray.Num());
	//}
	//else if (Slots.SlotArray.Num() == Slots.NumSlots)
	//{
	//	//Do nothing
	//}
	//else
	//{
	//	UE_LOGFMT(LogHGame, Error, "Downsizing array not yet implemented");
	//}
}

void UHItemSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, SlotStruct_Weapon_L);
	DOREPLIFETIME(ThisClass, SlotStruct_Weapon_R);
	DOREPLIFETIME(ThisClass, SlotStruct_Temporary);
}

void UHItemSlotComponent::UnequipItemInSlot(EHInventorySlotType SlotType)
{
	if (UHEquipmentComponent* EquipmentManager = FindEquipmentComponent())
	{
		UHEquipmentInstance* EquippedItem = GetSlotStructForEnum(SlotType).EquippedItem;

		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

void UHItemSlotComponent::EquipItemInSlot(EHInventorySlotType SlotType)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);


	check(Slots.SlotArray.IsValidIndex(Slots.ActiveSlotIndex));
	check(Slots.EquippedItem == nullptr);

	if (UHInventoryItemInstance* SlotItem = Slots.SlotArray[Slots.ActiveSlotIndex])
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
					Slots.EquippedItem = EquipmentManager->EquipItem(ItemDef->WeaponDefinition);

					if (Slots.EquippedItem != nullptr)
					{
						Slots.EquippedItem->SetInstigator(SlotItem);
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

void UHItemSlotComponent::Handle_OnRep_SlotsChanged(EHInventorySlotType SlotType)
{
	FHItemSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = GetSlotStructForEnum_Const(SlotType).SlotArray;
	Message.SlotType = SlotType;

	UE_LOGFMT(LogHGame, Warning, "This is happening!");

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(H_ItemSlots_Tags::TAG_ITEMSLOTS_MESSAGE_SLOTSCHANGED, Message);
}

void UHItemSlotComponent::Handle_OnRep_NumSlotsChanged(EHInventorySlotType SlotType)
{
	HandleResizeSlotArrayForEnum(SlotType);

	FHItemSlotsNumSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.NumSlots = GetSlotStructForEnum_Const(SlotType).NumSlots;
	Message.SlotType = SlotType;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(H_ItemSlots_Tags::TAG_ITEMSLOTS_MESSAGE_NUMSLOTSCHANGED, Message);
}

void UHItemSlotComponent::Handle_OnRep_ActiveSlotIndexChanged(EHInventorySlotType SlotType)
{
	FHItemSlotsActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = GetSlotStructForEnum_Const(SlotType).ActiveSlotIndex;
	Message.SlotType = SlotType;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(H_ItemSlots_Tags::TAG_ITEMSLOTS_MESSAGE_ACTIVEINDEXCHANGED, Message);
}

void UHItemSlotComponent::OnRep_SlotStruct_Weapon_L(FHInventorySlotStruct& PreviousValue)
{
	if (SlotStruct_Weapon_L.SlotArray != PreviousValue.SlotArray)
	{
		Handle_OnRep_SlotsChanged(EHInventorySlotType::Weapon_L);
	}

	if (SlotStruct_Weapon_L.NumSlots != PreviousValue.NumSlots)
	{
		Handle_OnRep_NumSlotsChanged(EHInventorySlotType::Weapon_L);
	}

	if (SlotStruct_Weapon_L.ActiveSlotIndex != PreviousValue.ActiveSlotIndex)
	{
		Handle_OnRep_ActiveSlotIndexChanged(EHInventorySlotType::Weapon_L);
	}
}

void UHItemSlotComponent::OnRep_SlotStruct_Weapon_R(FHInventorySlotStruct& PreviousValue)
{
	if (SlotStruct_Weapon_R.SlotArray != PreviousValue.SlotArray)
	{
		Handle_OnRep_SlotsChanged(EHInventorySlotType::Weapon_R);
	}

	if (SlotStruct_Weapon_R.NumSlots != PreviousValue.NumSlots)
	{
		Handle_OnRep_NumSlotsChanged(EHInventorySlotType::Weapon_R);
	}

	if (SlotStruct_Weapon_R.ActiveSlotIndex != PreviousValue.ActiveSlotIndex)
	{
		Handle_OnRep_ActiveSlotIndexChanged(EHInventorySlotType::Weapon_R);
	}
}

void UHItemSlotComponent::OnRep_SlotStruct_Temporary(FHInventorySlotStruct& PreviousValue)
{
	if (SlotStruct_Temporary.SlotArray != PreviousValue.SlotArray)
	{
		Handle_OnRep_SlotsChanged(EHInventorySlotType::Temporary);
	}

	if (SlotStruct_Temporary.NumSlots != PreviousValue.NumSlots)
	{
		Handle_OnRep_NumSlotsChanged(EHInventorySlotType::Temporary);
	}

	if (SlotStruct_Temporary.ActiveSlotIndex != PreviousValue.ActiveSlotIndex)
	{
		Handle_OnRep_ActiveSlotIndexChanged(EHInventorySlotType::Temporary);
	}
}

//ALSO NEED TO CHANGE CONST VERSION
FHInventorySlotStruct& UHItemSlotComponent::GetSlotStructForEnum(EHInventorySlotType SlotType)
{
	switch(SlotType)
	{
	case EHInventorySlotType::Weapon_L:
		return SlotStruct_Weapon_L;
	case EHInventorySlotType::Weapon_R:
		return SlotStruct_Weapon_R;
	case EHInventorySlotType::Temporary:
		return SlotStruct_Temporary;
	default:
		{
			UE_LOGFMT(LogHGame, Error, "Invalid enum in getslotstruct invcomp");
			return SlotStruct_Weapon_L;
		}
	}
}

//ALSO NEED TO CHANGE NONCONST VERSION
const FHInventorySlotStruct& UHItemSlotComponent::GetSlotStructForEnum_Const(EHInventorySlotType SlotType) const
{
	switch (SlotType)
	{
	case EHInventorySlotType::Weapon_L:
		return SlotStruct_Weapon_L;
	case EHInventorySlotType::Weapon_R:
		return SlotStruct_Weapon_R;
	case EHInventorySlotType::Temporary:
		return SlotStruct_Temporary;
	default:
	{
		UE_LOGFMT(LogHGame, Error, "Invalid enum in getslotstruct invcomp");
		return SlotStruct_Weapon_L;
	}
	}
}

