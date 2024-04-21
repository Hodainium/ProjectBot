// Fill out your copyright notice in the Description page of Project Settings.

#include "HPlayerState.h"
#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"
#include "HEquipmentComponent.h"
#include "HInventoryComponent.h"
#include "HItemSlotComponent.h"
#include "HPlayerCharacter.h"
#include "HWeaponComponent.h"


AHPlayerState::AHPlayerState()
{
	//Don't need to set anymore
	//NetUpdateFrequency = 100.f; //todo lower eventually but maybe not lyra uses 100
}
