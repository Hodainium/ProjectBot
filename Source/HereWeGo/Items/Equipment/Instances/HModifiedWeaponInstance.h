// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAbilitySet.h"
#include "HWeaponInstance.h"
#include "HereWeGo/Items/Modifiers/HItemModDefinition.h"
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

	

public:
	virtual void OnEquipped() override;

	virtual void OnUnequipped() override;

	void AddDamageGE(TSubclassOf<UGameplayEffect> Effect);

	void RemoveDamageGE(TSubclassOf<UGameplayEffect> Effect);

	void AddEffectOnHit(TSubclassOf<UGameplayEffect> Effect);

	void RemoveEffectOnHit(TSubclassOf<UGameplayEffect> Effect);

protected:

	//Authority only list of applied mods so we know which to remove on unequip
	UPROPERTY()
	TArray<FHItemModDef_GrantedHandles> AppliedModHandles;

	UPROPERTY()
	TArray<TSubclassOf<UGameplayEffect>> DamageGEArray;

	UPROPERTY()
	TArray<TSubclassOf<UGameplayEffect>> AdditionalEffectsToApplyOnHit;

	
};
