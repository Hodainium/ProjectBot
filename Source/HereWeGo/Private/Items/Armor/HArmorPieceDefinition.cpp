// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Armor/HArmorPieceDefinition.h"

#include "HereWeGo/Inventory/Armor/HArmorInstance.h"

UHArmorPieceDefinition::UHArmorPieceDefinition(const FObjectInitializer& ObjectInitializer)
{
	InstanceType = UHArmorInstance::StaticClass();
}
