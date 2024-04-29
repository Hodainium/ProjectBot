// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HItemDefinition.h"
#include "HereWeGo/HAssetManager.h"
#include "HWeaponItemDefinition.generated.h"

class UHEquipmentDefinition;
/**
 * 
 */
UCLASS()
class HEREWEGO_API UHWeaponItemDefinition : public UHItemDefinition
{
	GENERATED_BODY()

public:
	/** Constructor */
	UHWeaponItemDefinition()
	{
		bUsesAmmo = false;
		ItemType = EHItemType::Weapon;
		Damage = 0.f;
	}

	/** Weapon actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TObjectPtr<UHEquipmentDefinition> WeaponDefinition;

	/** Does this weapon use ammo? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bUsesAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	float Damage;


};
