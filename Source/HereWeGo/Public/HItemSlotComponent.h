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
		return GetSlotStructForEnum_Const(SlotType).SlotArray;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetActiveSlotIndexForEnum(EHWeaponSlotType SlotType) const
	{
		return GetSlotStructForEnum_Const(SlotType).ActiveSlotIndex;
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

	void Handle_OnRep_SlotsChanged(EHWeaponSlotType SlotType);

	void Handle_OnRep_NumSlotsChanged(EHWeaponSlotType SlotType);

	void Handle_OnRep_ActiveSlotIndexChanged(EHWeaponSlotType SlotType);

protected:

	UPROPERTY(ReplicatedUsing = OnRep_SlotStruct_Weapon_L)
	FHInventorySlotStruct SlotStruct_Weapon_L;

	UFUNCTION()
	void OnRep_SlotStruct_Weapon_L(FHInventorySlotStruct& PreviousValue);

	UPROPERTY(ReplicatedUsing = OnRep_SlotStruct_Weapon_R)
	FHInventorySlotStruct SlotStruct_Weapon_R;

	UFUNCTION()
	void OnRep_SlotStruct_Weapon_R(FHInventorySlotStruct& PreviousValue);

	UFUNCTION()
	FHInventorySlotStruct& GetSlotStructForEnum(EHWeaponSlotType SlotType);

	UFUNCTION()
	const FHInventorySlotStruct& GetSlotStructForEnum_Const(EHWeaponSlotType SlotType) const;
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
