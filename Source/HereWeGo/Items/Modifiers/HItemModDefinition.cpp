// Fill out your copyright notice in the Description page of Project Settings.


#include "HItemModDefinition.h"

#include "HAbilitySystemComponent.h"
#include "HereWeGo/Items/Equipment/Instances/HModifiedWeaponInstance.h"


void FHItemModDef_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FHItemModDef_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FHItemModDef_GrantedHandles::AddGameplayCue(const FGameplayEffectCue& InCue)
{
	GrantedPersistingCues.AppendTags(InCue.GameplayCueTags);
}

void FHItemModDef_GrantedHandles::RemoveMod(UHModifiedWeaponInstance* Instance)
{
	if(!(Instance->GetPawn() && Instance->GetPawn()->HasAuthority()))
	{
		return;
	}

	UHAbilitySystemComponent* HASC = Instance->GetASCFromOwningPawn();

	check(HASC);

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			HASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			HASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (const FGameplayTag& CueTag : GrantedPersistingCues)
	{
		HASC->RemoveGameplayCue(CueTag);
	}

	for (const FHItemModDef_DamageType& DamageType : GrantedDamageTypes)
	{
		Instance->RemoveDamageType(DamageType);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedPersistingCues.Reset();
}

UHItemModDefinition::UHItemModDefinition(const FObjectInitializer& ObjectInitializer)
{
}

void UHItemModDefinition::AddMod(UHModifiedWeaponInstance* Instance,
	FHItemModDef_GrantedHandles* OutGrantedHandles) const
{

}
