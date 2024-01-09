// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HGASTagContainers.generated.h"

class UHGASComponent;
DECLARE_MULTICAST_DELEGATE_TwoParams(FHOnGameplayTagCountChangeDelegate, const FGameplayTag, int32);
DECLARE_DELEGATE(FHDeferredTagChangeDelegate)


/**
 * 
 */
//USTRUCT(BlueprintType)
//struct HEREWEGO_API FHGameplayTagRequirements
//{
//	GENERATED_BODY() 
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FGameplayTagContainer RequiredTags;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	FGameplayTagContainer BlockingTags;
//
//	bool RequirementsMet(const FGameplayTagContainer& Container) const;
//
//	bool IsEmpty() const;
//
//	FString ToString() const;
//};

USTRUCT(BlueprintType)
struct HEREWEGO_API FHGameplayDynamicTags
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TagsToGrant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TagsToRemove;

	bool IsEmpty() const;

	FString ToString() const;
};

USTRUCT()
struct HEREWEGO_API FHGameplayTagCountContainer
{
	GENERATED_BODY()

private:
	struct FHTagDelegateInfo
	{
		FHOnGameplayTagCountChangeDelegate OnNewOrRemoveDelegate;
		FHOnGameplayTagCountChangeDelegate OnAnyChangeDelegate;

	};

	//UPROPERTY()
	TMap<FGameplayTag, FHTagDelegateInfo> TagEventMap;

public:

	UPROPERTY()
	TMap<FGameplayTag, int32> TagCountMap;

	

	FHOnGameplayTagCountChangeDelegate OnAnyTagChangeDelegate;

	bool UpdateTagCount(const FGameplayTag& Tag, const int32 DeltaCount);

	void UpdateTagCount(const FGameplayTagContainer& TagContainer, const int32 DeltaCount);

	void Notify_StackCountChange(const FGameplayTag& Tag);
};

//USTRUCT(BlueprintType)
//struct HEREWEGO_API FHActiveGameplayEffectHandle
//{
//	GENERATED_BODY()
//public:
//
//	FHActiveGameplayEffectHandle()
//		: Handle(INDEX_NONE), OwningComp(nullptr)
//	{
//	}
//
//	~FHActiveGameplayEffectHandle();
//
//	FHActiveGameplayEffectHandle(const FHActiveGameplayEffectHandle& Other) : FHActiveGameplayEffectHandle(Other.Handle, Other.OwningComp) {}
//
//	UHGASComponent* GetOwningGASComponent() const;
//
//	FHActiveGameplayEffectHandle(int NewHandle, UHGASComponent* OwningComponent) : Handle(NewHandle), OwningComp(OwningComponent) {}
//
//	static FHActiveGameplayEffectHandle GenerateNewHandle(UHGASComponent* OwningComponent);
//
//	UPROPERTY()
//	int Handle;
//
//	UPROPERTY()
//	UHGASComponent* OwningComp;
//
//	bool IsValid() const;
//
//	bool Equals(const FHActiveGameplayEffectHandle& Other) const
//	{
//		return Handle == Other.Handle;
//	}
//
//	bool operator==(const FHActiveGameplayEffectHandle& Other) const
//	{
//		return Equals(Other);
//	}
//
//	bool operator!=(const FHActiveGameplayEffectHandle& Other) const
//	{
//		return Handle != Other.Handle;
//	}
//
//	friend uint32 GetTypeHash(const FHActiveGameplayEffectHandle& HashObj)
//	{
//		uint32 Hash = FCrc::MemCrc32(&HashObj, sizeof(FHActiveGameplayEffectHandle));
//		return Hash;
//	}
//};

