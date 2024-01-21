// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"
#include "InventoryStructs.generated.h"

class UHEquipmentInstance;
class UHInventoryItemInstance;

USTRUCT()
struct FHSlotDataMap
{
	GENERATED_BODY()

	static TArray<TObjectPtr<UHInventoryItemInstance>> DefaultArray;
	static int32 DefaultInt;
	static TObjectPtr<UHEquipmentInstance> DefaultPtr;

	TArray<TObjectPtr<UHInventoryItemInstance>>* SlotsArray;

	int32* NumSlots;

	int32* ActiveSlotIndex;

	TObjectPtr<UHEquipmentInstance>* EquippedItem;

	bool bInitialized;

	FHSlotDataMap(): SlotsArray(nullptr), NumSlots(nullptr), ActiveSlotIndex(nullptr), EquippedItem(nullptr)
	{
		bInitialized = false;
	}

	FHSlotDataMap(TArray<TObjectPtr<UHInventoryItemInstance>>* InArrayPtr, int32* InNumSlotsPtr, int32* InActiveSlotIndexPtr, TObjectPtr<UHEquipmentInstance>* EquippedItemPtr)
	{
		SlotsArray = InArrayPtr;
		NumSlots = InNumSlotsPtr;
		ActiveSlotIndex = InActiveSlotIndexPtr;
		EquippedItem = EquippedItemPtr;

		if(InArrayPtr && InNumSlotsPtr && InActiveSlotIndexPtr && EquippedItemPtr)
		{
			bInitialized = true;
		}
		else
		{
			bInitialized = false;
		}
	}

	TArray<TObjectPtr<UHInventoryItemInstance>>& GetSlotsArray_Mutable()
	{
		if(SlotsArray && bInitialized)
		{
			return *SlotsArray;
		}
		else
		{
			UE_LOGFMT(LogHGame, Error, "Failed to retrieve slotsarray in FHSlotDataMap");
			return DefaultArray;
		}
	}

	const TArray<TObjectPtr<UHInventoryItemInstance>>& GetSlotsArray_NonMutable() const
	{
		if (SlotsArray && bInitialized)
		{
			return *SlotsArray;
		}
		else
		{
			UE_LOGFMT(LogHGame, Error, "Failed to retrieve slotsarray in FHSlotDataMap");
			return DefaultArray;
		}
	}

	int32& GetNumSlots_Mutable()
	{
		if (NumSlots && bInitialized)
		{
			return *NumSlots;
		}
		else
		{
			UE_LOGFMT(LogHGame, Error, "Failed to retrieve numSlots in FHSlotDataMap");
			return DefaultInt;
		}
	}

	const int32& GetNumSlots_NonMutable() const
	{
		if (NumSlots && bInitialized)
		{
			return *NumSlots;
		}
		else
		{
			UE_LOGFMT(LogHGame, Error, "Failed to retrieve numSlots in FHSlotDataMap");
			return DefaultInt;
		}
	}

	TObjectPtr<UHEquipmentInstance>& GetEquippedItem_Mutable()
	{
		if (NumSlots && bInitialized)
		{
			return *EquippedItem;
		}
		else
		{
			UE_LOGFMT(LogHGame, Error, "Failed to retrieve EquippedItem in FHSlotDataMap");
			return DefaultPtr;
		}
	}

	const TObjectPtr<UHEquipmentInstance>& GetEquippedItem_NonMutable() const
	{
		if (NumSlots && bInitialized)
		{
			return *EquippedItem;
		}
		else
		{
			UE_LOGFMT(LogHGame, Error, "Failed to retrieve EquippedItem in FHSlotDataMap");
			return DefaultPtr;
		}
	}
};

//USTRUCT(BlueprintType)
//struct FHInventorySlotStruct
//{
//	GENERATED_BODY()
//
//	UPROPERTY()
//	TArray<TObjectPtr<UHInventoryItemInstance>> SlotsArray;
//
//	UPROPERTY()
//	int32 NumSlots = 2;
//
//	UPROPERTY()
//	int32 ActiveSlotIndex = -1;
//
//	UPROPERTY(NotReplicated)
//	TObjectPtr<UHEquipmentInstance> EquippedItem;
//};

//void UHItemSlotComponent::OnRep_SlotStruct_Weapon_R(FHInventorySlotStruct& PreviousValue)
//{
//	if (SlotStruct_Weapon_R.SlotsArray != PreviousValue.SlotsArray)
//	{
//		BroadcastSlotsChanged(EHWeaponSlotType::Weapon_R);
//	}
//
//	if (SlotStruct_Weapon_R.NumSlots != PreviousValue.NumSlots)
//	{
//		BroadcastNumSlotsChanged(EHWeaponSlotType::Weapon_R);
//	}
//
//	if (SlotStruct_Weapon_R.ActiveSlotIndex != PreviousValue.ActiveSlotIndex)
//	{
//		BroadcastActiveSlotIndexChanged(EHWeaponSlotType::Weapon_R);
//	}
//}