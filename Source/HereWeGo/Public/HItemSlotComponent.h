// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HereWeGo/Inventory/InventoryStructs.h"
#include "HItemSlotComponent.generated.h"


class UHWeaponItemDefinition;
struct FHInventoryEntry;
class UHEquipmentComponent;
class UHEquipmentInstance;
class UHInventoryItemInstance;

// Delegate signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHOnReceivedServerSwapConfirmation, bool, bApproved);

UENUM(BlueprintType)
enum class EHInventorySlotType : uint8
{
	Weapon_L,
	Weapon_R,
	Temporary,
	Item
};
ENUM_RANGE_BY_FIRST_AND_LAST(EHInventorySlotType, EHInventorySlotType::Weapon_L, EHInventorySlotType::Item);

USTRUCT(BlueprintType)
struct FHNullEquipmentEntry
{
	GENERATED_BODY()

public:

	FHNullEquipmentEntry()
	{
		WeaponDefinition = nullptr;
		StackNumber = 0;
	}

	FHNullEquipmentEntry(UHWeaponItemDefinition* WeaponDef)
	{
		WeaponDefinition = WeaponDef;
		StackNumber = 1;
	}

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	TObjectPtr<UHWeaponItemDefinition> WeaponDefinition;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	uint8 StackNumber;
};

USTRUCT(BlueprintType)
struct FHInventorySlotIndex
{
	GENERATED_BODY()

public:

	FHInventorySlotIndex()
	{
		SlotType = EHInventorySlotType::Weapon_L;
		SlotIndex = 255;
	}

	bool operator==(const FHInventorySlotIndex& Other) const
	{
		return Equals(Other);
	}

	bool operator!=(const FHInventorySlotIndex& Other) const
	{
		return !Equals(Other);
	}

	bool Equals(const FHInventorySlotIndex& Other) const
	{
		return (SlotType == Other.SlotType) && (SlotIndex == Other.SlotIndex);
	}

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	EHInventorySlotType SlotType;

	UPROPERTY(BlueprintReadWrite, Category = Inventory)
	uint8 SlotIndex;
};

//The item slot component is in charge of equipping items
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HEREWEGO_API UHItemSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHItemSlotComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Slots")
	void RequestSwapOperation(FHInventorySlotIndex SourceIndex, FHInventorySlotIndex TargetIndex);

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
	UHInventoryItemInstance* GetItemAtIndex(FHInventorySlotIndex Index) const;

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

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	bool TryFindIndexForItem(UHInventoryItemInstance* ItemToFind, FHInventorySlotIndex& OutIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetNumSlotsForEnum(EHInventorySlotType SlotType, int32 InNum);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(EHInventorySlotType SlotType, int32 SlotIndex, UHInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddNullEquipment(UHWeaponItemDefinition* InEquipment);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveNullEquipment(UHWeaponItemDefinition* EquipmentToRemove);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveItemAtSlotIndex(EHInventorySlotType SlotType, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void TryRemoveItemFromSlots(UHInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, Category = "ItemSlots|Net")
	bool GetIsPendingServerConfirmation();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintAssignable)
	FHOnReceivedServerSwapConfirmation OnReceivedServerSwapConfirmation;

	

private:

	void UnequipItemInSlot(EHInventorySlotType SlotType);
	void EquipItemInSlot(EHInventorySlotType SlotType);

	void HandleNullEquipmentChange();

	UHEquipmentComponent* FindEquipmentComponent() const;

	void Handle_OnRep_SlotsChanged(EHInventorySlotType SlotType);

	void Handle_OnRep_NumSlotsChanged(EHInventorySlotType SlotType);

	void Handle_OnRep_ActiveSlotIndexChanged(EHInventorySlotType SlotType);

protected:

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = Inventory)
	void Server_SwapSlots(FHInventorySlotIndex SourceIndex, FHInventorySlotIndex TargetIndex);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = Inventory)
	void Client_SwapSlots(bool bWasSuccessful);

	//Flag that is set and removed when sending a server swap request and recieving confirmation
	UPROPERTY()
	bool IsPendingServerConfirmation;

	UPROPERTY(EditDefaultsOnly, Category = "ItemSlots|Defaults")
	int WeaponLStartingSlots = 0;

	UPROPERTY(EditDefaultsOnly, Category = "ItemSlots|Defaults")
	int WeaponRStartingSlots = 0;

	UPROPERTY(EditDefaultsOnly, Category = "ItemSlots|Defaults")
	int TemporaryStartingSlots = 0;

	UPROPERTY(EditDefaultsOnly, Category = "ItemSlots|Defaults")
	int ItemStartingSlots = 0;

	UPROPERTY(ReplicatedUsing = OnRep_SlotStruct_Weapon_L)
	FHInventorySlotStruct SlotStruct_Weapon_L;

	UFUNCTION()
	void OnRep_SlotStruct_Weapon_L(FHInventorySlotStruct& PreviousValue);

	UPROPERTY(ReplicatedUsing = OnRep_SlotStruct_Weapon_R)
	FHInventorySlotStruct SlotStruct_Weapon_R;

	UFUNCTION()
	void OnRep_SlotStruct_Weapon_R(FHInventorySlotStruct& PreviousValue);

	UPROPERTY(ReplicatedUsing = OnRep_SlotStruct_Temporary)
	FHInventorySlotStruct SlotStruct_Temporary;

	UFUNCTION()
	void OnRep_SlotStruct_Temporary(FHInventorySlotStruct& PreviousValue);

	UPROPERTY(ReplicatedUsing = OnRep_SlotStruct_Item)
	FHInventorySlotStruct SlotStruct_Item;

	UFUNCTION()
	void OnRep_SlotStruct_Item(FHInventorySlotStruct& PreviousValue);

	/*UPROPERTY(ReplicatedUsing = OnRep_SlotStruct_Item)
	FHInventorySlotStruct SlotStruct_Core;

	UFUNCTION()
	void OnRep_SlotStruct_Core(FHInventorySlotStruct& PreviousValue);*/

	UPROPERTY(Replicated)
	TArray<FHNullEquipmentEntry> NullEquipmentStack;

	UFUNCTION()
	FHInventorySlotStruct& GetSlotStructForEnum(EHInventorySlotType SlotType);

	UFUNCTION()
	const FHInventorySlotStruct& GetSlotStructForEnum_Const(EHInventorySlotType SlotType) const;
};

USTRUCT(BlueprintType)
struct FHItemSlotsChangedMessage
{
	GENERATED_BODY()

	FHItemSlotsChangedMessage() : SlotType(EHInventorySlotType::Weapon_L)
	{
	}

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

	FHItemSlotsActiveIndexChangedMessage() : SlotType(EHInventorySlotType::Weapon_L)
	{
	}

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

	FHItemSlotsNumSlotsChangedMessage(): SlotType(EHInventorySlotType::Weapon_L)
	{
	}

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	EHInventorySlotType SlotType;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NumSlots = 0;
};
