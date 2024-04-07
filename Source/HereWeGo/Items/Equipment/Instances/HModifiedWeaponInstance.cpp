// Fill out your copyright notice in the Description page of Project Settings.


#include "HModifiedWeaponInstance.h"

#include "HInventoryItemInstance.h"
#include "HLogChannels.h"
#include "HereWeGo/Items/Modifiers/HItemModInstance.h"
#include "Logging/StructuredLog.h"

void UHModifiedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	if(!(GetPawn() && GetPawn()->HasAuthority()))
	{
		return;
	}

	if(UHInventoryItemInstance* ItemInstance = Cast<UHInventoryItemInstance>(GetInstigator()))
	{
		//Untested but here we will grab mods from item instance and apply

		for (UHItemModInstance* Mod : ItemInstance->GetItemMods())
		{
			if (Mod != nullptr)
			{
				FHItemModDef_GrantedHandles GrantedHandles;
				Mod->OnWeaponEquipped(this, &GrantedHandles);
				AppliedModHandles.Add(GrantedHandles);
			}
		}
	}
}

void UHModifiedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();

	//For each mod in array remove mod

	if (!(GetPawn() && GetPawn()->HasAuthority()))
	{
		return;
	}

	if (UHInventoryItemInstance* ItemInstance = Cast<UHInventoryItemInstance>(GetInstigator()))
	{
		//Untested but here we will grab mods from item instance and apply

		for (FHItemModDef_GrantedHandles& Handle : AppliedModHandles)
		{
			Handle.RemoveModFromEquipmentInstance(this);
		}

		AppliedModHandles.Reset();
	}
}

void UHModifiedWeaponInstance::AddDamageGE(TSubclassOf<UGameplayEffect> Effect)
{
	DamageGEArray.Add(Effect);
}

void UHModifiedWeaponInstance::RemoveDamageGE(TSubclassOf<UGameplayEffect> Effect)
{
	DamageGEArray.RemoveSingle(Effect);
}

void UHModifiedWeaponInstance::AddEffectOnHit(TSubclassOf<UGameplayEffect> Effect)
{
	AdditionalEffectsToApplyOnHit.Add(Effect);
}

void UHModifiedWeaponInstance::RemoveEffectOnHit(TSubclassOf<UGameplayEffect> Effect)
{
	AdditionalEffectsToApplyOnHit.RemoveSingle(Effect);
}
