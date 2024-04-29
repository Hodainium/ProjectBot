// Fill out your copyright notice in the Description page of Project Settings.


#include "HAbilitySet.h"
#include "HGameplayAbility.h"
#include "HAbilitySystemComponent.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"

void FHAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FHAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FHAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FHAbilitySet_GrantedHandles::TakeFromAbilitySystem(UHAbilitySystemComponent* HASC)
{
	check(HASC)

	if(!HASC->IsOwnerActorAuthoritative())
	{
		//Must have authoirty to grant or take sets
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if(Handle.IsValid())
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

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		HASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UHAbilitySet::UHAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHAbilitySet::GiveToAbilitySystem(UHAbilitySystemComponent* HASC, FHAbilitySet_GrantedHandles* OutGrantedHandles,
	UObject* SourceObject) const
{
	check(HASC)

	if(!HASC->IsOwnerActorAuthoritative())
	{
		//Must have authoirty to grant or take sets
		return;
	}

	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); AbilityIndex++)
	{
		const FHAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if(!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("Granted Gameplay ability in index %d isnt valid"), AbilityIndex)
			continue;
		}

		UHGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UHGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = HASC->GiveAbility(AbilitySpec);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); EffectIndex++)
	{
		const FHAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("Granted Gameplay effect in index %d isnt valid"), EffectIndex)
			continue;
		}

		UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = HASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, HASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); SetIndex++)
	{
		const FHAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("Granted set in index %d isnt valid"), SetIndex)
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(HASC->GetOwner(), SetToGrant.AttributeSet);
		HASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}
