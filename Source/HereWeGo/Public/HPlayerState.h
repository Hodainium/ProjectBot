// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "HInventoryInterface.h"
#include "HWeaponInterface.h"
#include "ModularPlayerState.h"
#include "HPlayerState.generated.h"

class UAbilitySystemComponent;
class UHAbilitySystemComponent;
class UHWeaponComponent;
class UHAttributeSetBase;
struct FGameplayEffectSpec;

/**
 * 
 */
UCLASS()
class HEREWEGO_API AHPlayerState : public AModularPlayerState
{
	GENERATED_BODY()

public:
	AHPlayerState();

	//// Gets the replicated view rotation of this player, used for spectating
	//FRotator GetReplicatedViewRotation() const;

	//// Sets the replicated view rotation, only valid on the server
	//void SetReplicatedViewRotation(const FRotator& NewRotation);
	

protected:

protected:

};
