// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HItemModFragment.generated.h"

class UHModifiedWeaponInstance;
class UHItemModInstance;

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class HEREWEGO_API UHItemModFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void HandleOnEquipped(UHModifiedWeaponInstance* Equipment) const {}

	virtual void HandleOnUnequipped(UHModifiedWeaponInstance* Equipment) const {}
};


