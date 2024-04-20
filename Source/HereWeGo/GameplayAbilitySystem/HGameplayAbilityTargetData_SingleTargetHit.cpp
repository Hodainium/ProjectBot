// Fill out your copyright notice in the Description page of Project Settings.


#include "HGameplayAbilityTargetData_SingleTargetHit.h"

#include "HGameplayEffectContext.h"

struct FGameplayEffectContextHandle;

//////////////////////////////////////////////////////////////////////

void FHGameplayAbilityTargetData_SingleTargetHit::AddTargetDataToContext(FGameplayEffectContextHandle& Context, bool bIncludeActorArray) const
{
	FGameplayAbilityTargetData_SingleTargetHit::AddTargetDataToContext(Context, bIncludeActorArray);

	// Add game-specific data
	if (FHGameplayEffectContext* TypedContext = FHGameplayEffectContext::ExtractEffectContext(Context))
	{
		TypedContext->CartridgeID = CartridgeID;
	}
}

bool FHGameplayAbilityTargetData_SingleTargetHit::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayAbilityTargetData_SingleTargetHit::NetSerialize(Ar, Map, bOutSuccess);

	Ar << CartridgeID;

	return true;
}

