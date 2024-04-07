// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HItemModInstance.generated.h"

struct FHItemModDef_GrantedHandles;
class UHModifiedWeaponInstance;
class UHItemModDefinition;

UCLASS(BlueprintType)
class HEREWEGO_API UHItemModInstance : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UHItemModDefinition* GetModDefinition() const;

	UFUNCTION(BlueprintCallable)
	float GetModMagnitude() const;

	//TODO eventually make this accept Uobjects and check if they implement a certain interface?
	void OnWeaponEquipped(UHModifiedWeaponInstance* EquipmentInstance, FHItemModDef_GrantedHandles* OutGrantedHandles);
	
protected:

	/** ItemMod definition. The static data that this instance points to. */
	UPROPERTY()
	TObjectPtr<UHItemModDefinition> ModDef;

	UPROPERTY()
	float Magnitude;

	

	//void RemoveFromWeaponInstance(UHModifiedWeaponInstance* EquipmentInstance, FHItemModDef_GrantedHandles* GrantedHandles);
};