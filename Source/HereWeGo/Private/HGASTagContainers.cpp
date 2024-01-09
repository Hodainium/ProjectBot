// Fill out your copyright notice in the Description page of Project Settings.


#include "HGASTagContainers.h"


//bool FHGameplayTagRequirements::RequirementsMet(const FGameplayTagContainer& Container) const
//{
//	return Container.HasAll(RequiredTags) && !Container.HasAny(BlockingTags);
//}
//
//bool FHGameplayTagRequirements::IsEmpty() const
//{
//	return RequiredTags.IsEmpty() || BlockingTags.IsEmpty();
//}
//
//FString FHGameplayTagRequirements::ToString() const
//{
//	return TEXT("Tag requiremnts:: Required: %s \n Blocking: %s"), RequiredTags.ToString(), BlockingTags.ToString();
//}



bool FHGameplayDynamicTags::IsEmpty() const
{
	return TagsToGrant.IsEmpty() || TagsToRemove.IsEmpty();
}

FString FHGameplayDynamicTags::ToString() const
{
	return TEXT("Dynamic tags application:: Granted: %s \n Removed: %s"), TagsToGrant.ToString(), TagsToRemove.ToString();
}

bool FHGameplayTagCountContainer::UpdateTagCount(const FGameplayTag& Tag, const int32 DeltaCount)
{
	if (DeltaCount == 0)
	{
		return false;
	}

	bool bWasAddedOrRemoved = false;
	TArray<FHDeferredTagChangeDelegate> DeferredDelegates;

	bool bContainsTag = TagCountMap.Contains(Tag);
	if (!bContainsTag) //If we have the tag see if new count is negative, if so remove
	{
		if (DeltaCount > 0)
		{
			TagCountMap.Add(Tag);
		}
		else
		{
			//Do nothing. Can't add negative count to tag that is already gone
			return false;
		}
	}

	int32& TagCountRef = TagCountMap.FindOrAdd(Tag);

	int32 OldTagCount = TagCountRef;

	TagCountRef += DeltaCount;

	if (TagCountRef <= 0)
	{
		TagCountMap.Remove(Tag);
	}

	bWasAddedOrRemoved = (OldTagCount == 0 || TagCountRef <= 0);

	if(bWasAddedOrRemoved)
	{
		FHDeferredTagChangeDelegate NewDelegate = FHDeferredTagChangeDelegate::CreateLambda([Delegate = OnAnyTagChangeDelegate, Tag, TagCountRef]()
		{
			Delegate.Broadcast(Tag, TagCountRef);
		});
		DeferredDelegates.Add(NewDelegate);
	}

	FHTagDelegateInfo* DelegateInfo = TagEventMap.Find(Tag);

	if(DelegateInfo)
	{
		FHDeferredTagChangeDelegate NewOnAnyChangeDelegate = FHDeferredTagChangeDelegate::CreateLambda([Delegate = DelegateInfo->OnAnyChangeDelegate, Tag, TagCountRef]()
		{
			Delegate.Broadcast(Tag, TagCountRef);
		});
		DeferredDelegates.Add(NewOnAnyChangeDelegate);

		if(bWasAddedOrRemoved)
		{
			FHDeferredTagChangeDelegate NewOnNewOrRemoveDelegate = FHDeferredTagChangeDelegate::CreateLambda([Delegate = DelegateInfo->OnNewOrRemoveDelegate, Tag, TagCountRef]()
			{
				Delegate.Broadcast(Tag, TagCountRef);
			});
			DeferredDelegates.Add(NewOnNewOrRemoveDelegate);
		}
	}

	for (FHDeferredTagChangeDelegate Delegate : DeferredDelegates)
	{
		Delegate.Execute();
	}

	return bWasAddedOrRemoved;
}

void FHGameplayTagCountContainer::UpdateTagCount(const FGameplayTagContainer& TagContainer, const int32 DeltaCount)
{
	bool bUpdatedAny = false;
	for (const FGameplayTag& Tag : TagContainer)
	{
		bUpdatedAny |= UpdateTagCount(Tag, DeltaCount);
	}
}

void FHGameplayTagCountContainer::Notify_StackCountChange(const FGameplayTag& Tag)
{
	FHTagDelegateInfo* DelegateInfo = TagEventMap.Find(Tag);

	if(DelegateInfo)
	{
		int32 TagCount = TagCountMap.FindOrAdd(Tag);
		DelegateInfo->OnAnyChangeDelegate.Broadcast(Tag, TagCount);
	}
}

//FHActiveGameplayEffectHandle::~FHActiveGameplayEffectHandle()
//{
//}
//
//UHGASComponent* FHActiveGameplayEffectHandle::GetOwningGASComponent() const
//{
//	return OwningComp;
//}
//
//FHActiveGameplayEffectHandle FHActiveGameplayEffectHandle::GenerateNewHandle(UHGASComponent* OwningComponent)
//{
//	static int HandleCount = 0;
//
//	return FHActiveGameplayEffectHandle(HandleCount++, OwningComponent);
//}
//
//bool FHActiveGameplayEffectHandle::IsValid() const
//{
//	return Handle != INDEX_NONE;
//}


