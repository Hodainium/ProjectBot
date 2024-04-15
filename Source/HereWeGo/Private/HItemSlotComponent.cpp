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

	IsPendingServerConfirmation = false;
}

void UHItemSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		SetNumSlotsForEnum(EHInventorySlotType::Weapon_L, WeaponLStartingSlots);
		SetNumSlotsForEnum(EHInventorySlotType::Weapon_R, WeaponRStartingSlots);
		SetNumSlotsForEnum(EHInventorySlotType::Temporary, TemporaryStartingSlots);
		SetNumSlotsForEnum(EHInventorySlotType::Item, ItemStartingSlots);
	}
}

void UHItemSlotComponent::RequestSwapOperation(FHInventorySlotIndex SourceIndex, FHInventorySlotIndex TargetIndex)
{
	IsPendingServerConfirmation = true;
	Server_SwapSlots(SourceIndex, TargetIndex);
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

	//Get an empty slot. No other occupied slots are available
	SetActiveSlotIndexForEnum(SlotType, GetNextFreeItemSlot(SlotType));
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

	//Get an empty slot. No other occupied slots are available
	SetActiveSlotIndexForEnum(SlotType, GetNextFreeItemSlot(SlotType));
}

UHInventoryItemInstance* UHItemSlotComponent::GetItemAtIndex(FHInventorySlotIndex Index) const
{
	const FHInventorySlotStruct& Slots = GetSlotStructForEnum_Const(Index.SlotType);

	return Slots.SlotArray.IsValidIndex(Index.SlotIndex) ? Slots.SlotArray[Index.SlotIndex] : nullptr;
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

bool UHItemSlotComponent::TryFindIndexForItem(UHInventoryItemInstance* ItemToFind, FHInventorySlotIndex& OutIndex)
{
	for (EHInventorySlotType SlotType : TEnumRange<EHInventorySlotType>())
	{
		const FHInventorySlotStruct& Slots = GetSlotStructForEnum_Const(SlotType);

		for (int i = 0; i < Slots.SlotArray.Num(); i++)
		{
			if(ItemToFind == Slots.SlotArray[i])
			{
				OutIndex.SlotIndex = i;
				OutIndex.SlotType = SlotType;

				return true;
			}
		}
	}

	return false;
}

void UHItemSlotComponent::Server_SwapSlots_Implementation(FHInventorySlotIndex SourceIndex, FHInventorySlotIndex TargetIndex)
{
	FHInventorySlotStruct& SourceSlots = GetSlotStructForEnum(SourceIndex.SlotType);
	FHInventorySlotStruct& TargetSlots = GetSlotStructForEnum(TargetIndex.SlotType);

	bool bWasSuccessful = false;

	if (SourceSlots.SlotArray.IsValidIndex(SourceIndex.SlotIndex) && TargetSlots.SlotArray.IsValidIndex(TargetIndex.SlotIndex))
	{
		UHInventoryItemInstance* TempInstance = TargetSlots.SlotArray[TargetIndex.SlotIndex];

		TargetSlots.SlotArray[TargetIndex.SlotIndex] = SourceSlots.SlotArray[SourceIndex.SlotIndex];

		SourceSlots.SlotArray[SourceIndex.SlotIndex] = TempInstance;

		Handle_OnRep_SlotsChanged(SourceIndex.SlotType);
		Handle_OnRep_SlotsChanged(TargetIndex.SlotType);

		if(SourceSlots.ActiveSlotIndex == SourceIndex.SlotIndex)
		{
			UnequipItemInSlot(SourceIndex.SlotType);
			EquipItemInSlot(SourceIndex.SlotType);
		}

		if (TargetSlots.ActiveSlotIndex == TargetIndex.SlotIndex)
		{
			UnequipItemInSlot(TargetIndex.SlotType);
			EquipItemInSlot(TargetIndex.SlotType);
		}

		bWasSuccessful = true;
	}

	Client_SwapSlots_Implementation(bWasSuccessful);
}

void UHItemSlotComponent::Client_SwapSlots_Implementation(bool bWasSuccessful)
{
	IsPendingServerConfirmation = false;

	//Broadcast a message here
	if(OnReceivedServerSwapConfirmation.IsBound())
	{
		OnReceivedServerSwapConfirmation.Broadcast(bWasSuccessful);
	}
}

void UHItemSlotComponent::SetNumSlotsForEnum(EHInventorySlotType SlotType, int32 InNum)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);

	if(Slots.SlotArray.Num() == InNum)
	{
		return;
	}

	if (Slots.SlotArray.Num() < InNum)
	{
		Slots.SlotArray.AddDefaulted(InNum - Slots.SlotArray.Num());
	}
	else if (Slots.SlotArray.Num() > InNum)
	{
		//Downsizing array not yet implemented
		UE_LOGFMT(LogHGame, Warning, "Removing items but not yet dropping. Need to implement dropping items");
		//TODO maybe we can just send a gameplay event because dropping is handled via gas

		for (int i = 0; i < (Slots.SlotArray.Num() - InNum); i++)
		{
			int currentIdx = Slots.SlotArray.Num() - 1 - i;

			//Should drop items at these indexes
			Slots.SlotArray[currentIdx] = nullptr;
		}

		Slots.SlotArray.SetNum(InNum, true);

		//Set active index to be within bounds
		if(Slots.ActiveSlotIndex >= InNum)
		{
			SetActiveSlotIndexForEnum(SlotType, InNum - 1);
		}

		UE_LOGFMT(LogHGame, Warning, "Set num slots New raw size: {size}", Slots.SlotArray.Num());
	}

	UE_LOGFMT(LogHGame, Warning, "Num slots requesting size {size}", InNum);
	UE_LOGFMT(LogHGame, Warning, "Num slots og size {size}", Slots.SlotArray.Num());

	UE_LOGFMT(LogHGame, Warning, "Set num slots == {NUM}", InNum);

	Slots.NumSlots = InNum;
	
	Handle_OnRep_NumSlotsChanged(SlotType);
	Handle_OnRep_SlotsChanged(SlotType);
}

void UHItemSlotComponent::AddItemToSlot(EHInventorySlotType SlotType, int32 SlotIndex, UHInventoryItemInstance* Item)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);

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

void UHItemSlotComponent::RemoveItemAtSlotIndex(EHInventorySlotType SlotType, int32 SlotIndex)
{
	FHInventorySlotStruct& Slots = GetSlotStructForEnum(SlotType);

	UHInventoryItemInstance* Result = nullptr;

	//if (Slots.ActiveSlotIndex == SlotIndex)
	//{
	//	UnequipItemInSlot(SlotType);
	//	//Slots.ActiveSlotIndex = -1;
	//}

	if (Slots.SlotArray.IsValidIndex(SlotIndex))
	{
		Result = Slots.SlotArray[SlotIndex];

		if (Result != nullptr)
		{
			Slots.SlotArray[SlotIndex] = nullptr;
			Handle_OnRep_SlotsChanged(SlotType);
		}
	}

	//This should be safe to do after
	if(Slots.ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot(SlotType);

		//Now equip the null weapon
		EquipItemInSlot(SlotType);
	}
}

void UHItemSlotComponent::TryRemoveItemFromSlots(UHInventoryItemInstance* Item)
{
	FHInventorySlotIndex Index;
	if(TryFindIndexForItem(Item, Index))
	{
		RemoveItemAtSlotIndex(Index.SlotType, Index.SlotIndex);
	}
}

bool UHItemSlotComponent::GetIsPendingServerConfirmation()
{
	return IsPendingServerConfirmation;
}

void UHItemSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, SlotStruct_Weapon_L);
	DOREPLIFETIME(ThisClass, SlotStruct_Weapon_R);
	DOREPLIFETIME(ThisClass, SlotStruct_Temporary);
	DOREPLIFETIME(ThisClass, SlotStruct_Item);
	DOREPLIFETIME(ThisClass, NullEquipmentStack);
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
	//check(Slots.EquippedItem == nullptr);

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
					//Slots.EquippedItem = EquipmentManager->EquipItem(ItemDef->WeaponDefinition);
					Slots.EquippedItem = EquipmentManager->EquipItem(ItemDef->WeaponDefinition, SlotItem);
				}
			}

			break;
		}
		default:
		{
			UE_LOG(LogHGame, Error, TEXT("ITEMSLOTCOMP::Bruh you didn't include equipItemInSlot logic. For item: %s"), *SlotItem->GetItemDefinition()->ItemName.ToString());
		}
		}
	}
	else
	{
		//Search null equipment array for valid weapon def if so equip it.

		if(NullEquipmentStack.Num() > 0) 
		{
			UHWeaponItemDefinition* NullWeaponDef = nullptr; 

			for(int i = 0; i < NullEquipmentStack.Num(); i++)
			{
				if(NullEquipmentStack[NullEquipmentStack.Num() - 1 - i].WeaponDefinition != nullptr)
				{
					NullWeaponDef = NullEquipmentStack[NullEquipmentStack.Num() - 1 - i].WeaponDefinition;
					break;
				}
			}

			if(NullWeaponDef)
			{
				if (UHEquipmentComponent* EquipmentManager = FindEquipmentComponent())
				{
					//TODO create variants within null equipment entry for both left and right weapon slot

					//Slots.EquippedItem = EquipmentManager->EquipItem(ItemDef->WeaponDefinition);
					Slots.EquippedItem = EquipmentManager->EquipItem(NullWeaponDef->WeaponDefinition, SlotItem);
				}
			}
		}
	}
}

void UHItemSlotComponent::HandleNullEquipmentChange()
{
	FHInventorySlotStruct& LeftSlots = GetSlotStructForEnum(EHInventorySlotType::Weapon_L);
	FHInventorySlotStruct& RightSlots = GetSlotStructForEnum(EHInventorySlotType::Weapon_R);

	if(LeftSlots.SlotArray.IsValidIndex(LeftSlots.ActiveSlotIndex) && LeftSlots.SlotArray[LeftSlots.ActiveSlotIndex] == nullptr)
	{
		UnequipItemInSlot(EHInventorySlotType::Weapon_L);
		EquipItemInSlot(EHInventorySlotType::Weapon_L);
	}

	if (RightSlots.SlotArray.IsValidIndex(RightSlots.ActiveSlotIndex) && RightSlots.SlotArray[RightSlots.ActiveSlotIndex] == nullptr)
	{
		UnequipItemInSlot(EHInventorySlotType::Weapon_R);
		EquipItemInSlot(EHInventorySlotType::Weapon_R);
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
	FHItemSlotsNumSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.NumSlots = GetSlotStructForEnum_Const(SlotType).NumSlots;
	Message.SlotType = SlotType;

	UE_LOGFMT(LogHGame, Error, "Sending numslots message: {typ}", Message.NumSlots);

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

void UHItemSlotComponent::OnRep_SlotStruct_Item(FHInventorySlotStruct& PreviousValue)
{
	if (SlotStruct_Temporary.SlotArray != PreviousValue.SlotArray)
	{
		Handle_OnRep_SlotsChanged(EHInventorySlotType::Item);
	}

	if (SlotStruct_Temporary.NumSlots != PreviousValue.NumSlots)
	{
		Handle_OnRep_NumSlotsChanged(EHInventorySlotType::Item);
	}

	if (SlotStruct_Temporary.ActiveSlotIndex != PreviousValue.ActiveSlotIndex)
	{
		Handle_OnRep_ActiveSlotIndexChanged(EHInventorySlotType::Item);
	}
}

void UHItemSlotComponent::AddNullEquipment(UHWeaponItemDefinition* InEquipment)
{
	bool bFound = false;
	for (FHNullEquipmentEntry& Entry : NullEquipmentStack)
	{
		if(Entry.WeaponDefinition == InEquipment)
		{
			Entry.StackNumber += 1;
			bFound = true;
			break;
		}
	}

	if(!bFound)
	{
		NullEquipmentStack.Add(FHNullEquipmentEntry(InEquipment));
	}

	HandleNullEquipmentChange();
}

void UHItemSlotComponent::RemoveNullEquipment(UHWeaponItemDefinition* EquipmentToRemove)
{
	for (FHNullEquipmentEntry& Entry : NullEquipmentStack)
	{
		if (Entry.WeaponDefinition == EquipmentToRemove)
		{
			Entry.StackNumber -= 1;
			HandleNullEquipmentChange();
			return;
		}
	}

	UE_LOGFMT(LogHGame, Error, "Tried to remove null equipment that didnt exist");
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
	case EHInventorySlotType::Item:
		return SlotStruct_Item;
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
	case EHInventorySlotType::Item:
		return SlotStruct_Item;
	default:
	{
		UE_LOGFMT(LogHGame, Error, "Invalid enum in getslotstruct invcomp");
		return SlotStruct_Weapon_L;
	}
	}
}

