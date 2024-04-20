// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "HGameplayAbilityTargetData_SingleTargetHit.generated.h"

/**
 * 
 */
USTRUCT()
struct FHGameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()

	FHGameplayAbilityTargetData_SingleTargetHit()
		: CartridgeID(-1)
	{ }

	virtual void AddTargetDataToContext(FGameplayEffectContextHandle& Context, bool bIncludeActorArray) const override;

	/** ID to allow the identification of multiple bullets that were part of the same cartridge */
	UPROPERTY()
	int32 CartridgeID;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FHGameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}
};

template<>
struct TStructOpsTypeTraits<FHGameplayAbilityTargetData_SingleTargetHit> : public TStructOpsTypeTraitsBase2<FHGameplayAbilityTargetData_SingleTargetHit>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};