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
				Mod->HandleOnEquipped(this);
				AppliedMods.Add(Mod);
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

	UHAbilitySystemComponent* HASC = GetASCFromOwningPawn();
	check(HASC)

	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			HASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GrantedGameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			HASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		HASC->RemoveSpawnedAttribute(Set);
	}

	for (const FGameplayTag& CueTag : GrantedPersistingCues)
	{
		HASC->RemoveGameplayCue(CueTag);
	}

	DamageGEArray.Reset();

	GrantedAbilitySpecHandles.Reset();
	GrantedGameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
	GrantedPersistingCues.Reset();

	AppliedMods.Reset();
}
