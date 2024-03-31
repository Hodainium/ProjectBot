// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HWeaponModDataTableRow.generated.h"

/**
 * 
 */
USTRUCT()
struct HEREWEGO_API FHWeaponModDataTableRow : public FTableRowBase
{
	GENERATED_BODY()
public:
	FHWeaponModDataTableRow()
	{
		Weight = 1.f;
	}

	UPROPERTY(EditDefaultsOnly, meta = (AllowedTypes = "ItemMod_Type"))
	FPrimaryAssetId ItemModDef;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0))
	float Weight;
};
