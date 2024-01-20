// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HItemSlotComponent.generated.h"


class UHEquipmentComponent;
class UHEquipmentInstance;
class UHInventoryItemInstance;

UENUM(BlueprintType)
enum class EHSlotType : uint8
{
	Weapon_L,
	Weapon_R,
	Armor_Head,
	Armor_Chest,
	Armor_ArmL,
	Armor_ArmR,
	Armor_LegL,
	Armor_LegR,
	Armor_Core
};

//The item slot component is in charge of equipping items
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEREWEGO_API UHItemSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHItemSlotComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Slots")
	void CycleActiveSlotForward(EHSlotType SlotType);

	UFUNCTION(BlueprintCallable, Category = "Slots")
	void CycleActiveSlotBackward(EHSlotType SlotType);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Slots")
	void SetActiveSlotIndex(EHSlotType SlotType, int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	TArray<UHInventoryItemInstance*> GetSlots(EHSlotType SlotType) const
	{
		TArray<UHInventoryItemInstance*> OutArray;
		GetSlotArrayForEnum(SlotType, OutArray);

		return OutArray;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetActiveSlotIndex(EHSlotType SlotType) const
	{
		int32 ActiveSlotIndex;
		GetActiveSlotIndexForEnum(SlotType, ActiveSlotIndex);
		return ActiveSlotIndex;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UHInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, UHInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UHInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void UnequipItemInSlot(EHSlotType SlotType);
	void EquipItemInSlot(EHSlotType SlotType);

	UHEquipmentComponent* FindEquipmentComponent() const;

protected:
	UPROPERTY()
	int32 NumWeaponSlots_L = 2;

	UPROPERTY()
	int32 NumWeaponSlots_R = 2;

	UPROPERTY()
	int32 NumArmorSlots = 1;

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
	
	//Armor Head/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Armor_Head)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Armor_Head;

	UFUNCTION()
	void OnRep_Slots_Armor_Head();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Armor_Head)
	int32 NumSlots_Armor_Head = 1;

	UFUNCTION()
	void OnRep_NumSlots_Armor_Head();
	
	//Armor Chest/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Armor_Chest)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Armor_Chest;

	UFUNCTION()
	void OnRep_Slots_Armor_Chest();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Armor_Chest)
	int32 NumSlots_Armor_Chest = 1;

	UFUNCTION()
	void OnRep_NumSlots_Armor_Chest();

	//Armor Arm Left/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Armor_ArmL)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Armor_ArmL;

	UFUNCTION()
	void OnRep_Slots_Armor_ArmL();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Armor_ArmL)
	int32 NumSlots_Armor_ArmL = 1;

	UFUNCTION()
	void OnRep_NumSlots_Armor_ArmL();

	//Armor Arm Right/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Armor_ArmR)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Armor_ArmR;

	UFUNCTION()
	void OnRep_Slots_Armor_ArmR();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Armor_ArmR)
	int32 NumSlots_Armor_ArmR = 1;

	UFUNCTION()
	void OnRep_NumSlots_Armor_ArmR();

	//Armor Leg Left/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Armor_LegL)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Armor_LegL;

	UFUNCTION()
	void OnRep_Slots_Armor_LegL();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Armor_LegL)
	int32 NumSlots_Armor_LegL = 1;

	UFUNCTION()
	void OnRep_NumSlots_Armor_LegL();

	//Armor Leg Right/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Armor_LegR)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Armor_LegR;

	UFUNCTION()
	void OnRep_Slots_Armor_LegR();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Armor_LegR)
	int32 NumSlots_Armor_LegR = 1;

	UFUNCTION()
	void OnRep_NumSlots_Armor_LegR();

	//Armor Core/////////////////////////////////////////////////////

	UPROPERTY(ReplicatedUsing = OnRep_Slots_Armor_Core)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots_Armor_Core;

	UFUNCTION()
	void OnRep_Slots_Armor_Core();

	UPROPERTY(ReplicatedUsing = OnRep_NumSlots_Armor_Core)
	int32 NumSlots_Armor_Core = 1;

	UFUNCTION()
	void OnRep_NumSlots_Armor_Core();

	//End/////////////////////////////////////////////////////

	UFUNCTION()
	bool GetSlotArrayForEnum(EHSlotType SlotType, TArray<UHInventoryItemInstance*>& OutSlotArray) const;

	UFUNCTION()
	bool GetNumSlotsForEnum(EHSlotType SlotType, int32& OutNumSlots);

	UFUNCTION()
	bool GetActiveSlotIndexForEnum(EHSlotType SlotType, int32& OutActiveSlotIndex) const;

	UFUNCTION()
	bool Trigger_OnRep_Slots_ForEnum(EHSlotType SlotType);

	UFUNCTION()
	bool Trigger_OnRep_NumSlots_ForEnum(EHSlotType SlotType);

	UFUNCTION()
	bool Trigger_OnRep_ActiveSlotIndex_ForEnum(EHSlotType SlotType);
	

	UPROPERTY()
	TObjectPtr<UHEquipmentInstance> EquippedItem;
};

USTRUCT(BlueprintType)
struct FHQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<UHInventoryItemInstance>> Slots;
};

USTRUCT(BlueprintType)
struct FHQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 ActiveIndex = 0;
};
