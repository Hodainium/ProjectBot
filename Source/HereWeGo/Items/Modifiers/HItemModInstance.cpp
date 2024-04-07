// Fill out your copyright notice in the Description page of Project Settings.


#include "HItemModInstance.h"

#include "HItemModDefinition.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

UHItemModDefinition* UHItemModInstance::GetModDefinition() const
{
	return ModDef;
}

EHItemQuality UHItemModInstance::GetModQuality() const
{
	return ModQuality;
}

float UHItemModInstance::GetModMagnitude() const
{
	return Magnitude;
}

void UHItemModInstance::OnWeaponEquipped(UHModifiedWeaponInstance* EquipmentInstance,
                                            FHItemModDef_GrantedHandles* OutGrantedHandles)
{
	if (!EquipmentInstance || !ModDef)
	{
		UE_LOGFMT(LogHGame, Error, "Attempted to add mods to item instance but equipment or moddef were null");
		return;
	}

	ModDef->ApplyToEquipmentInstance(EquipmentInstance, OutGrantedHandles, Magnitude);
}

void UHItemModInstance::SetModMagnitude(float inMagnitude)
{
	Magnitude = inMagnitude;
}

void UHItemModInstance::SetModDefinition(UHItemModDefinition* InDef)
{
	ModDef = InDef;
}

void UHItemModInstance::SetModQuality(EHItemQuality InQuality)
{
	ModQuality = InQuality;
}

//void UHItemModInstance::RemoveFromWeaponInstance(UHModifiedWeaponInstance* EquipmentInstance,
//	FHItemModDef_GrantedHandles* GrantedHandles)
//{
//	if (!EquipmentInstance || !ModDef)
//	{
//		UE_LOGFMT(LogHGame, Error, "Attempted to remove mods to item instance but equipment or moddef were null");
//		return;
//	}
//
//	if(GrantedHandles)
//	{
//		GrantedHandles->RemoveModFromEquipmentInstance(EquipmentInstance);
//	}
//}
