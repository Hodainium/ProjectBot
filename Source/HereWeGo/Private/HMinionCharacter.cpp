// Fill out your copyright notice in the Description page of Project Settings.


#include "HMinionCharacter.h"

#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"
#include "HEquipmentComponent.h"
#include "HInventoryComponent.h"
#include "HItemSlotComponent.h"
#include "HWeaponComponent.h"
#include "Components/CapsuleComponent.h"

AHMinionCharacter::AHMinionCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

