// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "NativeGameplayTags.h"
#include "HereWeGo/Inventory/Components/HGridInventoryComponent.h"
#include "HereWeGo/Inventory/Grid/HInventoryGrid.h"
#include "HInventoryComponent.generated.h"


class UHGridArray;
class UHGridInventoryComponent;
class UHGridItem;
class UHItemDefinition;
class UHInventoryComponent;
struct FHInventoryList;
class UHInventoryItemInstance;

//UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Inventory_Item_Count, "ItemStatTags.Inventory.Item.Count");

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FHInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UHInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 Delta = 0;
};

//USTRUCT(BlueprintType)
//struct FHInventorySize
//{
//	GENERATED_BODY()
//
//	FHInventorySize()
//	{
//		SizeX = MAX_uint8;
//		SizeY = MAX_uint8;
//	}
//
//	UPROPERTY(BlueprintReadOnly)
//	uint8 SizeX;
//
//	UPROPERTY(BlueprintReadOnly)
//	uint8 SizeY;
//
//	bool IsValid()
//	{
//		return !(SizeX == MAX_uint8 || SizeY == MAX_uint8);
//	}
//};

USTRUCT(BlueprintType)
struct FHInventoryPoint
{
	GENERATED_BODY()

	FHInventoryPoint()
	{
		X = MAX_uint8;
		Y = MAX_uint8;
	}

	FHInventoryPoint(int32 InX, int32 InY) : X(InX), Y(InY)
	{}

	UPROPERTY(BlueprintReadOnly)
	uint8 X;

	UPROPERTY(BlueprintReadOnly)
	uint8 Y;

	bool operator==(const FHInventoryPoint& Other) const
	{
		return Equals(Other);
	}

	bool operator!=(const FHInventoryPoint& Other) const
	{
		return !Equals(Other);
	}

	bool Equals (const FHInventoryPoint& Other) const
	{
		return (X == Other.X && Y == Other.Y);
	}

public:
	FString ToString() const
	{
		return TEXT("({X},{Y})", X, Y);
	}

	bool IsValid() const
	{
		return !(X == MAX_uint8 || Y == MAX_uint8);
	}

	FHInventoryPoint operator+(const FHInventoryPoint& Other) const
	{
		return FHInventoryPoint(this->X + Other.X, this->Y + Other.Y);
	}
};

USTRUCT(BlueprintType)
struct FHInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FHInventoryEntry()
	{
	}

	FHInventoryEntry(UHInventoryItemInstance* InInstance, FHInventoryPoint InPoint, bool bIsRotated, int32 InStackCount) : Instance(InInstance),
		TopLeftTilePoint(InPoint), IsRotated(bIsRotated), StackCount(InStackCount)
	{
	}

	FHInventoryEntry(UHGridItem* Item)
	{
		if(Item)
		{
			Instance = Item->Instance;
			TopLeftTilePoint = Item->TopLeftTilePoint;
			IsRotated = Item->IsRotated;
			StackCount = Item->StackCount;
		}
		
	}

	FString GetDebugString() const;

private:
	friend FHInventoryList;
	friend UHInventoryComponent;
	friend UHGridInventoryComponent;
	friend UHGridItem;
	friend UHGridArray;
	friend UHGridInventoryComponent;

private:
	UPROPERTY()
	TObjectPtr<UHInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	FHInventoryPoint TopLeftTilePoint;

	UPROPERTY()
	bool IsRotated = false;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FHInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FHInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FHInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UHInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	void PostReplicatedReceive(const FPostReplicatedReceiveParameters& Parameters);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FHInventoryEntry, FHInventoryList>(Entries, DeltaParms, *this);
	}

	UHInventoryItemInstance* AddEntry(UHItemDefinition* ItemDef, int32 StackCount);
	void AddEntry(UHInventoryItemInstance* Instance);

	void AddEntry(FHInventoryEntry& InEntry);

	void RemoveEntry(UHInventoryItemInstance* Instance);

	bool MarkItemIDDirty(int32 ItemID);
	

private:
	friend UHInventoryComponent;
	friend UHGridInventoryComponent;

	void BroadcastChangeMessage(FHInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FHInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FHInventoryList> : public TStructOpsTypeTraitsBase2<FHInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};

UCLASS(BlueprintType)
class HEREWEGO_API UHInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanStackItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	UHInventoryItemInstance* AddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddItemInstance(UHInventoryItemInstance* ItemInstance);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveItemInstance(UHInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure = false)
	TArray<UHInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
	UHInventoryItemInstance* FindFirstItemStackByDefinition(UHItemDefinition* ItemDef) const;

	int32 GetTotalItemCountByDefinition(UHItemDefinition* ItemDef) const;
	bool ConsumeItemsByDefinition(UHItemDefinition* ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FHInventoryList InventoryList;

	UPROPERTY()
	TArray<TObjectPtr<UHInventoryItemInstance>> InventoryGrid;
};
