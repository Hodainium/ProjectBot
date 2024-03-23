// Fill out your copyright notice in the Description page of Project Settings.


#include "HModifiedWeaponInstance.h"

#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

void UHModifiedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	if(UHInventoryItemInstance* ItemInstance = Cast<UHInventoryItemInstance>(GetInstigator()))
	{
		//Untested but here we will grab mods from item instance and apply
	}
}

void UHModifiedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();

	//For each mod in array perform removemod
}
