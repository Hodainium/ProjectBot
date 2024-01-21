// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HereWeGo/Inventory/InventoryStructs.h"
#include "HItemSlotComponent.generated.h"


struct FHInventoryEntry;
class UHEquipmentComponent;
class UHEquipmentInstance;
class UHInventoryItemInstance;

UENUM(BlueprintType)
enum class EHWeaponSlotType : uint8
{
	Weapon_L,
	Weapon_R
};
ENUM_RANGE_BY_FIRST_AND_LAST(EHWeaponSlotType, EHWeaponSlotType::Weapon_L, EHWeaponSlotType::Weapon_R);

//The item slot component is in charge of equipping items
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HEREWEGO_API UHItemSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHItemSlotComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Slots")
	void CycleActiveSlotForward(EHWeaponSlotType SlotType);

	UFUNCTION(BlueprintCallable, Category = "Slots")
	void CycleActiveSlotBackward(EHWeaponSlotType SlotType);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Slots")
	void SetActiveSlotIndexForEnum(EHWeaponSlotType SlotType, int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	TArray<UHInventoryItemInstance*> GetSlotsForEnum(EHWeaponSlotType SlotType) const
	{
		return GetSlotArrayForItemSlotEnum_NonMutable(SlotType);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetActiveSlotIndexForEnum(EHWeaponSlotType SlotType) const
	{
		return GetActiveSlotIndexForItemSlotEnum_NonMutable(SlotType);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UHInventoryItemInstance* GetActiveSlotItem(EHWeaponSlotType SlotType) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetNextFreeItemSlot(EHWeaponSlotType SlotType) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(EHWeaponSlotType SlotType, int32 SlotIndex, UHInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UHInventoryItemInstance* RemoveItemFromSlot(EHWeaponSlotType SlotType, int32 SlotIndex);

	virtual void BeginPlay() override;
	void HandleResizeSlotArrayForEnum(EHWeaponSlotType SlotType);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	void UnequipItemInSlot(EHWeaponSlotType SlotType);
	void EquipItemInSlot(EHWeaponSlotType SlotType);

	UHEquipmentComponent* FindEquipmentComponent() const;

	void BroadcastSlotsChanged(EHWeaponSlotType SlotType);

	void BroadcastNumSlotsChanged(EHWeaponSlotType SlotType);

	void BroadcastActiveSlotIndexChanged(EHWeaponSlotType SlotType) const;

	int32 EnumToIndex(EHWeaponSlotType Enum);

protected:

	//Weapon L/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Weapon_L)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Weapon_L;

	UFUNCTION()
	void OnRep_Slots_Weapon_L();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Weapon_L)
	int32 NumSlots_Weapon_L = 2;

	UFUNCTION()
	void OnRep_NumSlots_Weapon_L();

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex_Weapon_L)
	int32 ActiveSlotIndex_Weapon_L = -1;

	UFUNCTION()
	void OnRep_ActiveSlotIndex_Weapon_L();

	UPROPERTY()
	TObjectPtr<UHEquipmentInstance> EquippedItem_Weapon_L;


	//Weapon R/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Weapon_R)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Weapon_R;

	UFUNCTION()
	void OnRep_Slots_Weapon_R();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Weapon_R)
	int32 NumSlots_Weapon_R = 1;

	UFUNCTION()
	void OnRep_NumSlots_Weapon_R();

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex_Weapon_R)
	int32 ActiveSlotIndex_Weapon_R = -1;

	UFUNCTION()
	void OnRep_ActiveSlotIndex_Weapon_R();

	UPROPERTY()
	TObjectPtr<UHEquipmentInstance> EquippedItem_Weapon_R;

	//End/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex_Weapon_L)
	FHInventorySlotStruct SlotStruct_Weapon_L;

	void OnRep_SlotStruct_Weapon_L(FHInventorySlotStruct& PreviousValue);

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex_Weapon_R)
	FHInventorySlotStruct SlotStruct_Weapon_R;

	void OnRep_SlotStruct_Weapon_R(FHInventorySlotStruct& PreviousValue);

	//UFUNCTION()
	bool Trigger_OnRep_Slots_ForEnum(EHWeaponSlotType SlotType);

	//UFUNCTION()
	bool Trigger_OnRep_NumSlots_ForEnum(EHWeaponSlotType SlotType);

	//UFUNCTION()
	bool Trigger_OnRep_ActiveSlotIndex_ForEnum(EHWeaponSlotType SlotType);
};

USTRUCT(BlueprintType)
struct FHItemSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	EHWeaponSlotType SlotType;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots;
};

USTRUCT(BlueprintType)
struct FHItemSlotsActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	EHWeaponSlotType SlotType;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 ActiveIndex = 0;
};

USTRUCT(BlueprintType)
struct FHItemSlotsNumSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	EHWeaponSlotType SlotType;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NumSlots = 0;
};
