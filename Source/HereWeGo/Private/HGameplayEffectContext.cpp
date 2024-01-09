// Fill out your copyright notice in the Description page of Project Settings.


#include "HGameplayEffectContext.h"

#include "HAbilitySourceInterface.h"

class FArchive;

FHGameplayEffectContext* FHGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FHGameplayEffectContext::StaticStruct()))
	{
		return (FHGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FHGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

void FHGameplayEffectContext::SetAbilitySource(const IHAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IHAbilitySourceInterface* FHGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IHAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FHGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}