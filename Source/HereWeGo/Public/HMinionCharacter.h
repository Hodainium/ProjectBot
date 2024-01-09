// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HCharacterBase.h"
#include "HMinionCharacter.generated.h"

class UHEquipmentComponent;
class UHItemSlotComponent;
class UHAbilitySystemComponent;
class UHAttributeSetBase;
class UHWeaponComponent;
class UHInventoryComponent;


/**
 * 
 */
UCLASS()
class HEREWEGO_API AHMinionCharacter : public AHCharacterBase
{
	GENERATED_BODY()

public:
	AHMinionCharacter(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY()
	TObjectPtr<UHAbilitySystemComponent> AbilitySystemComponentHardRef;

	UPROPERTY()
	TObjectPtr<UHAttributeSetBase> AttributeSetBaseHardRef;

	UPROPERTY()
	TObjectPtr<UHWeaponComponent> WeaponComponentHardRef;

	UPROPERTY()
	TObjectPtr<UHInventoryComponent> InventoryComponentHardRef;

	UPROPERTY()
	TObjectPtr<UHEquipmentComponent> EquipmentComponentHardRef;

	UPROPERTY()
	TObjectPtr<UHItemSlotComponent> ItemSlotComponentHardRef;

	virtual void BeginPlay() override;

	
};
