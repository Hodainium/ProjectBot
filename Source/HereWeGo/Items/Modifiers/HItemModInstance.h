// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UHModifiedWeaponInstance;
class UHItemModDefinition;

UCLASS()
class HEREWEGO_API UHItemModInstance : public UObject
{
	GENERATED_BODY()

public:
	/** ItemMod definition. The static data that this instance points to. */
	UPROPERTY()
	TObjectPtr<const UHItemModDefinition> ModDef;

	UPROPERTY()
	float Magnitude;

	void HandleOnEquipped(UHModifiedWeaponInstance* Equipment);

	void HandleOnUnequipped(UHModifiedWeaponInstance* Item);
};