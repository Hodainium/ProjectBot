// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HAbilitySet.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "HEquipmentComponent.generated.h"


class UHInventoryItemInstance;
struct FHEquipmentList;
class UHEquipmentComponent;
class UHEquipmentDefinition;
class UHEquipmentInstance;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FHAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FHAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FHEquipmentList;
	friend UHEquipmentComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TObjectPtr<UHEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UHEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FHAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FHEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FHEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FHEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FHAppliedEquipmentEntry, FHEquipmentList>(Entries, DeltaParms, *this);
	}

	UHEquipmentInstance* AddEntry(UHEquipmentDefinition* EquipmentDefinition);
	void RemoveEntry(UHEquipmentInstance* Instance);

private:
	UHAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UHEquipmentComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FHAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FHEquipmentList> : public TStructOpsTypeTraitsBase2<FHEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};

/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class HEREWEGO_API UHEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHEquipmentComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UHEquipmentInstance* EquipItem(UHEquipmentDefinition* EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UHEquipmentInstance* EquipmentInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UHEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UHEquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UHEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UHEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FHEquipmentList EquipmentList;


};
