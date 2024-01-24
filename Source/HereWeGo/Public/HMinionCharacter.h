// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HCharacterBaseOld.h"
#include "HereWeGo/Actors/Characters/HCharacterBase.h"
#include "HMinionCharacter.generated.h"

class UHEquipmentComponent;
class UHItemSlotComponent;
class UHAbilitySystemComponent;
class UHAttributeSetBase;
class UHWeaponComponent;
class UHInventoryComponent;


/**
 * Only difference for right now is that we turn off camera collision and set replicated mode to minimal for asc
 * ALSO is lacking some components that a playable char would have like a camera
 */
UCLASS()
class HEREWEGO_API AHMinionCharacter : public AHCharacterBase
{
	GENERATED_BODY()

public:
	AHMinionCharacter(const FObjectInitializer& ObjectInitializer);

protected:
};
