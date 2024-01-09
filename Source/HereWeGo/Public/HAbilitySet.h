// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "HAbilitySet.generated.h"

class UGameplayEffect;
class UAttributeSet;
class UHAbilitySystemComponent;
class UHGameplayAbility;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FHAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHGameplayAbility> Ability = nullptr;

	// Level of ability to grant
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


USTRUCT(BlueprintType)
struct FHAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effct
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct FHAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:

	// Set to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;

};

USTRUCT(BlueprintType)
struct FHAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UHAbilitySystemComponent* HASC);

protected:
	//Handles to granted abilties
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	//Handles to granted effects
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	//Pointers to granted sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

UCLASS(BlueprintType, Const)
class HEREWEGO_API UHAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UHAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GiveToAbilitySystem(UHAbilitySystemComponent* HASC, FHAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities")
	TArray<FHAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FHAbilitySet_GameplayEffect> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FHAbilitySet_AttributeSet> GrantedAttributes;
};
