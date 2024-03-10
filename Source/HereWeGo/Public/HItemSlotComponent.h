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
enum class EHInventorySlotType : uint8
{
	Weapon_L,
	Weapon_R,
	Temporary
};
ENUM_RANGE_BY_FIRST_AND_LAST(EHInventorySlotType, EHInventorySlotType::Weapon_L, EHInventorySlotType::Weapon_R);

//The item slot component is in charge of equipping items
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HEREWEGO_API UHItemSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHItemSlotComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Slots")
	void CycleActiveSlotForward(EHInventorySlotType SlotType);

	UFUNCTION(BlueprintCallable, Category = "Slots")
	void CycleActiveSlotBackward(EHInventorySlotType SlotType);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Slots")
	void SetActiveSlotIndexForEnum(EHInventorySlotType SlotType, int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	TArray<UHInventoryItemInstance*> GetSlotsForEnum(EHInventorySlotType SlotType) const
	{
		return GetSlotStructForEnum_Const(SlotType).SlotArray;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetNumSlotsForEnum(EHInventorySlotType SlotType) const
	{
		return GetSlotStructForEnum_Const(SlotType).NumSlots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetActiveSlotIndexForEnum(EHInventorySlotType SlotType) const
	{
		return GetSlotStructForEnum_Const(SlotType).ActiveSlotIndex;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UHInventoryItemInstance* GetActiveSlotItem(EHInventorySlotType SlotType) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	int32 GetNextFreeItemSlot(EHInventorySlotType SlotType) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(EHInventorySlotType SlotType, int32 SlotIndex, UHInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UHInventoryItemInstance* RemoveItemFromSlot(EHInventorySlotType SlotType, int32 SlotIndex);

	virtual void BeginPlay() override;
	void HandleResizeSlotArrayForEnum(EHInventorySlotType SlotType);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	void UnequipItemInSlot(EHInventorySlotType SlotType);
	void EquipItemInSlot(EHInventorySlotType SlotType);

	UHEquipmentComponent* FindEquipmentComponent() const;

	void Handle_OnRep_SlotsChanged(EHInventorySlotType SlotType);

	void Handle_OnRep_NumSlotsChanged(EHInventorySlotType SlotType);

	void Handle_OnRep_ActiveSlotIndexChanged(EHInventorySlotType SlotType);

protected:

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SlotStruct_Weapon_L)
	FHInventorySlotStruct SlotStruct_Weapon_L;

	UFUNCTION()
	void OnRep_SlotStruct_Weapon_L(FHInventorySlotStruct& PreviousValue);

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SlotStruct_Weapon_R)
	FHInventorySlotStruct SlotStruct_Weapon_R;

	UFUNCTION()
	void OnRep_SlotStruct_Weapon_R(FHInventorySlotStruct& PreviousValue);

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_SlotStruct_Temporary)
	FHInventorySlotStruct SlotStruct_Temporary;

	UFUNCTION()
	void OnRep_SlotStruct_Temporary(FHInventorySlotStruct& PreviousValue);

	UFUNCTION()
	FHInventorySlotStruct& GetSlotStructForEnum(EHInventorySlotType SlotType);

	UFUNCTION()
	const FHInventorySlotStruct& GetSlotStructForEnum_Const(EHInventorySlotType SlotType) const;
};

USTRUCT(BlueprintType)
struct FHItemSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	EHInventorySlotType SlotType;

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
	EHInventorySlotType SlotType;

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
	EHInventorySlotType SlotType;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NumSlots = 0;
};
