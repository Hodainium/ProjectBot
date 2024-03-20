// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HWeaponInstance.h"
#include "HModifiedWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHModifiedWeaponInstance : public UHWeaponInstance
{
	GENERATED_BODY()

	virtual void OnEquipped() override;

	virtual void OnUnequipped() override;
};
