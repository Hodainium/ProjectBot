// Fill out your copyright notice in the Description page of Project Settings.


#include "HItemModInstance.h"

#include "HItemModDefinition.h"

void UHItemModInstance::HandleOnEquipped(UHModifiedWeaponInstance* Equipment)
{
	if (!Equipment || !ModDef)
	{
		return;
	}

	/*for (UHItemModFragment* Fragment : ModDef->Fragments)
	{
		Fragment->HandleOnEquipped(Equipment);
	}*/
}

void UHItemModInstance::HandleOnUnequipped(UHModifiedWeaponInstance* Equipment)
{
	if (!Equipment || !ModDef)
	{
		return;
	}

	/*for (UHItemModFragment* Fragment : ModDef->Fragments)
	{
		Fragment->HandleOnUnequipped(Equipment);
	}*/
}