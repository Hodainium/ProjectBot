// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"
#include "InventoryStructs.generated.h"

class UHEquipmentInstance;
class UHInventoryItemInstance;

USTRUCT(BlueprintType)
struct FHInventorySlotStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<UHInventoryItemInstance>> SlotArray;

	UPROPERTY()
	int32 NumSlots = 2;

	UPROPERTY()
	int32 ActiveSlotIndex = -1;

	UPROPERTY(NotReplicated)
	TObjectPtr<UHEquipmentInstance> EquippedItem;
};
//
//USTRUCT()
//struct FHSlotDataEntry
//{
//	GENERATED_BODY()
//
//	friend struct FHSlotDataMapContainer;
//
//public:
//
//	FHSlotDataEntry(): SlotsArray(nullptr), NumSlots(nullptr), ActiveSlotIndex(nullptr), EquippedItem(nullptr)
//	{
//		bInitialized = false;
//	}
//
//	FHSlotDataEntry(TArray<TObjectPtr<UHInventoryItemInstance>>* InArrayPtr, int32* InNumSlotsPtr, int32* InActiveSlotIndexPtr, TObjectPtr<UHEquipmentInstance>* EquippedItemPtr)
//	{
//		SlotsArray = InArrayPtr;
//		NumSlots = InNumSlotsPtr;
//		ActiveSlotIndex = InActiveSlotIndexPtr;
//		EquippedItem = EquippedItemPtr;
//
//		if(InArrayPtr && InNumSlotsPtr && InActiveSlotIndexPtr && EquippedItemPtr)
//		{
//			bInitialized = true;
//		}
//		else
//		{
//			bInitialized = false;
//		}
//	}
//
//private:
//
//	static TArray<TObjectPtr<UHInventoryItemInstance>> DefaultArray;
//	static int32 DefaultInt;
//	static TObjectPtr<UHEquipmentInstance> DefaultPtr;
//
//	TArray<TObjectPtr<UHInventoryItemInstance>>* SlotsArray;
//
//	int32* NumSlots;
//
//	int32* ActiveSlotIndex;
//
//	TObjectPtr<UHEquipmentInstance>* EquippedItem;
//
//	bool bInitialized;
//
//	TArray<TObjectPtr<UHInventoryItemInstance>>& GetSlotsArray_Mutable()
//	{
//		if(SlotsArray && bInitialized)
//		{
//			return *SlotsArray;
//		}
//		else
//		{
//			UE_LOGFMT(LogHGame, Error, "Failed to retrieve slotsarray in FHSlotDataEntry");
//			return DefaultArray;
//		}
//	}
//
//	const TArray<TObjectPtr<UHInventoryItemInstance>>& GetSlotsArray_NonMutable() const
//	{
//		if (SlotsArray && bInitialized)
//		{
//			return *SlotsArray;
//		}
//		else
//		{
//			UE_LOGFMT(LogHGame, Error, "Failed to retrieve slotsarray in FHSlotDataEntry");
//			return DefaultArray;
//		}
//	}
//
//	int32& GetNumSlots_Mutable()
//	{
//		if (NumSlots && bInitialized)
//		{
//			return *NumSlots;
//		}
//		else
//		{
//			UE_LOGFMT(LogHGame, Error, "Failed to retrieve numSlots in FHSlotDataEntry");
//			return DefaultInt;
//		}
//	}
//
//	const int32& GetNumSlots_NonMutable() const
//	{
//		if (NumSlots && bInitialized)
//		{
//			return *NumSlots;
//		}
//		else
//		{
//			UE_LOGFMT(LogHGame, Error, "Failed to retrieve numSlots in FHSlotDataEntry");
//			return DefaultInt;
//		}
//	}
//
//	int32& GetActiveSlotIndex_Mutable()
//	{
//		if (NumSlots && bInitialized)
//		{
//			return *ActiveSlotIndex;
//		}
//		else
//		{
//			UE_LOGFMT(LogHGame, Error, "Failed to retrieve numSlots in FHSlotDataEntry");
//			return DefaultInt;
//		}
//	}
//
//	const int32& GetActiveSlotIndex_NonMutable() const
//	{
//		if (NumSlots && bInitialized)
//		{
//			return *ActiveSlotIndex;
//		}
//		else
//		{
//			UE_LOGFMT(LogHGame, Error, "Failed to retrieve numSlots in FHSlotDataEntry");
//			return DefaultInt;
//		}
//	}
//
//	TObjectPtr<UHEquipmentInstance>& GetEquippedItem_Mutable()
//	{
//		if (NumSlots && bInitialized)
//		{
//			return *EquippedItem;
//		}
//		else
//		{
//			UE_LOGFMT(LogHGame, Error, "Failed to retrieve EquippedItem in FHSlotDataEntry");
//			return DefaultPtr;
//		}
//	}
//
//	const TObjectPtr<UHEquipmentInstance>& GetEquippedItem_NonMutable() const
//	{
//		if (NumSlots && bInitialized)
//		{
//			return *EquippedItem;
//		}
//		else
//		{
//			UE_LOGFMT(LogHGame, Error, "Failed to retrieve EquippedItem in FHSlotDataEntry");
//			return DefaultPtr;
//		}
//	}
//};
//
//USTRUCT()
//struct FHSlotDataMapContainer
//{
//	GENERATED_BODY()
//
//	static TArray<TObjectPtr<UHInventoryItemInstance>> DefaultArray;
//	static int32 DefaultInt;
//	static TObjectPtr<UHEquipmentInstance> DefaultPtr;
//
//	TArray<FHSlotDataEntry> Entries;
//
//	FHSlotDataMapContainer()
//	{
//		DefaultInt = INDEX_NONE;
//		DefaultPtr = nullptr;
//	}
//
//	void AddEntry(FHSlotDataEntry InEntry)
//	{
//		if(InEntry.bInitialized)
//		{
//			Entries.Add(InEntry);
//		}
//	}
//
//	bool IsValidEnum(EHWeaponSlotType Enum)
//	{
//		return Entries.IsValidIndex(EnumToInt(Enum));
//	}
//
//	inline int32 EnumToInt(EHWeaponSlotType Enum) const
//	{
//		return static_cast<int32>(Enum);
//	}
//
//	TArray<TObjectPtr<UHInventoryItemInstance>>& GetSlotsArray_Mutable(EHWeaponSlotType SlotType)
//	{
//		int32 Index = EnumToInt(SlotType);
//
//		if(Entries.IsValidIndex(Index) && Entries[Index].bInitialized)
//		{
//			return Entries[Index].GetSlotsArray_Mutable();
//		}
//		else
//		{
//			return DefaultArray;
//		}
//	}
//
//	const TArray<TObjectPtr<UHInventoryItemInstance>>& GetSlotsArray_NonMutable(EHWeaponSlotType SlotType) const
//	{
//		int32 Index = EnumToInt(SlotType);
//
//		if (Entries.IsValidIndex(Index) && Entries[Index].bInitialized)
//		{
//			return Entries[Index].GetSlotsArray_NonMutable();
//		}
//		else
//		{
//			return DefaultArray;
//		}
//	}
//
//	int32& GetNumSlots_Mutable(EHWeaponSlotType SlotType)
//	{
//		int32 Index = EnumToInt(SlotType);
//
//		if (Entries.IsValidIndex(Index) && Entries[Index].bInitialized)
//		{
//			return Entries[Index].GetNumSlots_Mutable();
//		}
//		else
//		{
//			return DefaultInt;
//		}
//	}
//
//	const int32& GetNumSlots_NonMutable(EHWeaponSlotType SlotType) const
//	{
//		int32 Index = EnumToInt(SlotType);
//
//		if (Entries.IsValidIndex(Index) && Entries[Index].bInitialized)
//		{
//			return Entries[Index].GetNumSlots_NonMutable();
//		}
//		else
//		{
//			return DefaultInt;
//		}
//	}
//
//	int32& GetActiveSlotIndex_Mutable(EHWeaponSlotType SlotType)
//	{
//		int32 Index = EnumToInt(SlotType);
//
//		if (Entries.IsValidIndex(Index) && Entries[Index].bInitialized)
//		{
//			return Entries[Index].GetActiveSlotIndex_Mutable();
//		}
//		else
//		{
//			return DefaultInt;
//		}
//	}
//
//	const int32& GetActiveSlotIndex_NonMutable(EHWeaponSlotType SlotType) const
//	{
//		int32 Index = EnumToInt(SlotType);
//
//		if (Entries.IsValidIndex(Index) && Entries[Index].bInitialized)
//		{
//			return Entries[Index].GetActiveSlotIndex_NonMutable();
//		}
//		else
//		{
//			return DefaultInt;
//		}
//	}
//
//	TObjectPtr<UHEquipmentInstance>& GetEquippedItem_Mutable(EHWeaponSlotType SlotType)
//	{
//		int32 Index = EnumToInt(SlotType);
//
//		if (Entries.IsValidIndex(Index) && Entries[Index].bInitialized)
//		{
//			return Entries[Index].GetEquippedItem_Mutable();
//		}
//		else
//		{
//			return DefaultPtr;
//		}
//	}
//
//	const TObjectPtr<UHEquipmentInstance>& GetEquippedItem_NonMutable(EHWeaponSlotType SlotType) const
//	{
//		int32 Index = EnumToInt(SlotType);
//
//		if (Entries.IsValidIndex(Index) && Entries[Index].bInitialized)
//		{
//			return Entries[Index].GetEquippedItem_NonMutable();
//		}
//		else
//		{
//			return DefaultPtr;
//		}
//	}
//};



