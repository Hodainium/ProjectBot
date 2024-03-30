// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HItemModInstance.generated.h"

struct FHItemModDef_GrantedHandles;
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

	//TODO eventually make this accept Uobjects and check if they implement a certain interface?
	void AddToWeaponInstance(UHModifiedWeaponInstance* EquipmentInstance, FHItemModDef_GrantedHandles* OutGrantedHandles);

	//void RemoveFromWeaponInstance(UHModifiedWeaponInstance* EquipmentInstance, FHItemModDef_GrantedHandles* GrantedHandles);
};