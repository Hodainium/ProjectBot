// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "HGameplayTagStackContainer.generated.h"

struct FHGameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

/**
 * Represents one stack of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct FHGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FHGameplayTagStack()
	{}

	FHGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{
	}

	FString GetDebugString() const;

private:
	friend FHGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

/** Container of gameplay tag stacks */
USTRUCT(BlueprintType)
struct FHGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FHGameplayTagStackContainer()
		//	: Owner(nullptr)
	{
	}

public:
	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	// Returns true if there is at least one stack of the specified tag
	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FHGameplayTagStack, FHGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FHGameplayTagStack> Stacks;

	// Accelerated list of tag stacks for queries
	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FHGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FHGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};