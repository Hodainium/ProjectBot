// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HItemModInstance.generated.h"

enum class EHItemQuality : uint8;
struct FHItemModDef_GrantedHandles;
class UHModifiedWeaponInstance;
class UHItemModDefinition;

UCLASS(BlueprintType)
class HEREWEGO_API UHItemModInstance : public UObject
{
	GENERATED_BODY()

public:
	UHItemModInstance();

	UFUNCTION(BlueprintCallable)
	UHItemModDefinition* GetModDefinition() const;

	UFUNCTION(BlueprintCallable)
	EHItemQuality GetModQuality() const;

	UFUNCTION(BlueprintCallable, Category="Mods|Display")
	float GetDisplayMagnitude() const;

	UFUNCTION(BlueprintCallable, Category = "Mods|Display")
	FText GetDisplayDescription() const;

	UFUNCTION(BlueprintCallable)
	float GetModLevel() const;

	//TODO eventually make this accept Uobjects and check if they implement a certain interface?
	void OnWeaponEquipped(UHModifiedWeaponInstance* EquipmentInstance, FHItemModDef_GrantedHandles* OutGrantedHandles);
	
protected:

	friend class ULootGenGameInstanceSubsystem;

	void SetModLevelOffset(float inLevelOffset);

	void SetModDefinition(UHItemModDefinition* InDef);

	void SetModQuality(EHItemQuality InQuality);

	/** ItemMod definition. The static data that this instance points to. */
	UPROPERTY()
	TObjectPtr<UHItemModDefinition> ModDef;

	UPROPERTY()
	float LevelOffset;

	UPROPERTY()
	EHItemQuality ModQuality;

	//void RemoveFromWeaponInstance(UHModifiedWeaponInstance* EquipmentInstance, FHItemModDef_GrantedHandles* GrantedHandles);
};