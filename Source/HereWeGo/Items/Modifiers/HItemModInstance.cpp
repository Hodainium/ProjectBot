// Fill out your copyright notice in the Description page of Project Settings.


#include "HItemModInstance.h"

#include "HItemDefinition.h"
#include "HItemModDefinition.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

UHItemModInstance::UHItemModInstance() 
{
	ModQuality = EHItemQuality::Quality0;
	LevelOffset = 0;
}

UHItemModDefinition* UHItemModInstance::GetModDefinition() const
{
	return ModDef;
}

EHItemQuality UHItemModInstance::GetModQuality() const
{
	return ModQuality;
}

float UHItemModInstance::GetDisplayMagnitude() const
{
	if(ModDef)
	{
		return ModDef->GetDisplayMagnitude(GetModLevel());
	}
	return -2;
}

float UHItemModInstance::GetModLevel() const
{
	return static_cast<int>(ModQuality) + 1 + LevelOffset;
}

void UHItemModInstance::OnWeaponEquipped(UHModifiedWeaponInstance* EquipmentInstance,
                                            FHItemModDef_GrantedHandles* OutGrantedHandles)
{
	if (!EquipmentInstance || !ModDef)
	{
		UE_LOGFMT(LogHGame, Error, "Attempted to add mods to item instance but equipment or moddef were null");
		return;
	}

	ModDef->ApplyToEquipmentInstance(EquipmentInstance, OutGrantedHandles, GetModLevel());
}

void UHItemModInstance::SetModLevelOffset(float inLevelOffset)
{
	LevelOffset = inLevelOffset;
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
