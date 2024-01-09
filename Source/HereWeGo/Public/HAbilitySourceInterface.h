// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HAbilitySourceInterface.generated.h"

struct FGameplayTagContainer;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHAbilitySourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HEREWEGO_API IHAbilitySourceInterface
{
	GENERATED_BODY()

	/**
	 * Compute the multiplier for effect falloff with distance
	 *
	 * @param Distance			Distance from source to target for ability calculations (distance bullet traveled for a gun, etc...)
	 * @param SourceTags		Aggregated Tags from the source
	 * @param TargetTags		Aggregated Tags currently on the target
	 *
	 * @return Multiplier to apply to the base attribute value due to distance
	 */
	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;

	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;
};
