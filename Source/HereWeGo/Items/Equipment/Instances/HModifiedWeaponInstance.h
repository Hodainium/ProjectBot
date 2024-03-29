// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAbilitySet.h"
#include "HWeaponInstance.h"
#include "HModifiedWeaponInstance.generated.h"

class UAttributeSet;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UHItemModInstance;
/**
 * 
 */
UCLASS()
class HEREWEGO_API UHModifiedWeaponInstance : public UHWeaponInstance
{
	GENERATED_BODY()

	virtual void OnEquipped() override;

	virtual void OnUnequipped() override;

public:
	UPROPERTY()
	TArray<TSubclassOf<UGameplayEffect>> DamageGEArray;

protected:

	//Authority only list of applied mods so we know which to remove on unequip
	UPROPERTY()
	TArray<TObjectPtr<UHItemModInstance>> AppliedMods;

	
};
