// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HItemModDefinition.h"
#include "HItemModFragment.h"
#include "HItemModFragmentTypes.generated.h"

class UGameplayEffect;

USTRUCT()
struct HEREWEGO_API FHModFragmentData_GameplayEffect
{
	GENERATED_BODY()

	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHItemModFragment_ApplyGameplayEffects : public UHItemModFragment
{
	GENERATED_BODY()

	TArray<>

	void HandleOnEquipped(UHModifiedWeaponInstance* Equipment) const override;

	void HandleOnUnequipped(UHModifiedWeaponInstance* Equipment) const override;
};

UCLASS(BlueprintType)
class HEREWEGO_API UHItemModFragment_AddGameplayCues_Persistent : public UHItemModFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer PersistentCues;

	void HandleOnEquipped(UHModifiedWeaponInstance* Equipment) const override;

	void HandleOnUnequipped(UHModifiedWeaponInstance* Equipment) const override;
};

UCLASS(BlueprintType)
class HEREWEGO_API UHItemModFragment_AddGameplayCues_OnHitBurst : public UHItemModFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer OnHitBurstCues;

	void HandleOnEquipped(UHModifiedWeaponInstance* Equipment) const override;

	void HandleOnUnequipped(UHModifiedWeaponInstance* Equipment) const override;
};

UCLASS(BlueprintType)
class HEREWEGO_API UHItemModFragment_AddGameplayCues_OnPickUpBurst : public UHItemModFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer OnPickupBurstCues;

	void HandleOnEquipped(UHModifiedWeaponInstance* Equipment) const override;

	void HandleOnUnequipped(UHModifiedWeaponInstance* Equipment) const override;
};


