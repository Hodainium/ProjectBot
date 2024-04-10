// Fill out your copyright notice in the Description page of Project Settings.


#include "HItemModDefinition.h"

#include "HAbilitySystemComponent.h"
#include "HLogChannels.h"
#include "HereWeGo/HAssetManager.h"
#include "HereWeGo/GameplayEffects/HGameplayEffectUIData_Magnitude.h"
#include "HereWeGo/Items/Equipment/Instances/HModifiedWeaponInstance.h"
#include "Logging/StructuredLog.h"


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

void FHItemModDef_GrantedHandles::AddGrantedDamageGE(const FHItemModDef_DamageType& Effect)
{
	GrantedDamageTypes.Add(Effect);
}

void FHItemModDef_GrantedHandles::AddGrantedOnHitGE(const FHItemModDef_GameplayEffect& Effect)
{
	GrantedEffectsToApplyOnHit.Add(Effect);
}

void FHItemModDef_GrantedHandles::AddGrantedTexture(const FHItemModDef_TextureOverride& Tex)
{
	GrantedTextureOverrides.Add(Tex);
}

void FHItemModDef_GrantedHandles::AddActorSpawned(AActor* Actor)
{
	ExtraActorsSpawned.Add(Actor);
}

void FHItemModDef_GrantedHandles::RemoveModFromEquipmentInstance(UHModifiedWeaponInstance* Instance)
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

	for (const FHItemModDef_GameplayEffect& Effect : GrantedEffectsToApplyOnHit)
	{
		Instance->RemoveEffectOnHit(Effect.GameplayEffect);
	}

	for (const FHItemModDef_DamageType& DamageType : GrantedDamageTypes)
	{
		Instance->RemoveDamageGE(DamageType.DamageGE);
	}

	UE_LOGFMT(LogHGame, Warning, "Need to implement texture override add and remove");
	/*for (const FHItemModDef_TextureOverride& Tex : GrantedTextureOverrides)
	{
		
	}*/

	UE_LOGFMT(LogHGame, Warning, "Need to implement extraActors add and remove");
	/*for(AActor* Actor : ExtraActorsSpawned)
	{
		
	}*/

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedPersistingCues.Reset();
	GrantedEffectsToApplyOnHit.Reset();
	GrantedDamageTypes.Reset();
	GrantedTextureOverrides.Reset();
	ExtraActorsSpawned.Reset();
}

UHItemModDefinition::UHItemModDefinition(const FObjectInitializer& ObjectInitializer)
{
}

void UHItemModDefinition::ApplyToEquipmentInstance(UHModifiedWeaponInstance* Instance,
	FHItemModDef_GrantedHandles* OutGrantedHandles, int Magnitude) const
{
	if (!(Instance->GetPawn() && Instance->GetPawn()->HasAuthority()))
	{
		return;
	}

	UHAbilitySystemComponent* HASC = Instance->GetASCFromOwningPawn();

	check(HASC);

	UE_LOGFMT(LogHGame, Warning, "Need to implement extraActors add and remove");
	/*for(AActor* Actor : ExtraActorsSpawned)
	{

	}*/

	for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilitiesToGrant.Num(); AbilityIndex++)
	{
		const FHItemModDef_GameplayAbility& AbilityToGrant = GameplayAbilitiesToGrant[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("Granted Mod Gameplay ability in index %d isnt valid"), AbilityIndex)
				continue;
		}

		UHGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UHGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, Magnitude);
		AbilitySpec.SourceObject = Instance;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = HASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	for (int32 EffectIndex = 0; EffectIndex < GameplayEffectsToGrant.Num(); EffectIndex++)
	{
		const FHItemModDef_GameplayEffect& EffectToGrant = GameplayEffectsToGrant[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("Granted mod Gameplay effect in index %d isnt valid"), EffectIndex)
				continue;
		}

		UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = HASC->ApplyGameplayEffectToSelf(GameplayEffect, Magnitude, HASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	for (int32 CueIndex = 0; CueIndex < GameplayCuesToPlay.Num(); CueIndex++)
	{
		const FHItemModDef_GameplayCue& CueToPlay = GameplayCuesToPlay[CueIndex];

		for (const FGameplayTag& Tag : CueToPlay.GameplayCue.GameplayCueTags)
		{
			UE_LOG(LogTemp, Error, TEXT("Adding mod cue but Need to give gameplay cues context"))
			HASC->AddGameplayCue(Tag, HASC->MakeEffectContext());
		}

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayCue(CueToPlay.GameplayCue);
		}
	}

	for (int32 AbilityIndex = 0; AbilityIndex < EffectsToApplyOnHit.Num(); AbilityIndex++)
	{
		const FHItemModDef_GameplayEffect& EffectToApply = EffectsToApplyOnHit[AbilityIndex];

		if (!IsValid(EffectToApply.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("Granted Mod Gameplay effect on hit in index %d isnt valid"), AbilityIndex)
				continue;
		}

		Instance->AddEffectOnHit(EffectToApply.GameplayEffect);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGrantedOnHitGE(EffectToApply);
		}
	}

	for (int32 AbilityIndex = 0; AbilityIndex < DamageTypesToGrant.Num(); AbilityIndex++)
	{
		const FHItemModDef_DamageType& DamageType = DamageTypesToGrant[AbilityIndex];

		if (!IsValid(DamageType.DamageGE))
		{
			UE_LOG(LogTemp, Error, TEXT("Granted Mod Gameplay effect on hit in index %d isnt valid"), AbilityIndex)
				continue;
		}

		Instance->AddDamageGE(DamageType.DamageGE);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGrantedDamageGE(DamageType);
		}
	}
}

FPrimaryAssetId UHItemModDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(UHAssetManager::ItemModItemType, GetFName());
}

float UHItemModDefinition::GetDisplayMagnitude(float InLevel)
{
	if(bUseGEMagnitude)
	{
		if(GameplayEffectsToGrant.IsValidIndex(EffectDisplayMagnitudeIndex.DisplayGEIndex))
		{
			if (const UGameplayEffect* GE = GetDefault<UGameplayEffect>(GameplayEffectsToGrant[EffectDisplayMagnitudeIndex.DisplayGEIndex].GameplayEffect))
			{
				if(const UHGameplayEffectUIData_Magnitude* DataComp = GE->FindComponent<UHGameplayEffectUIData_Magnitude>())
				{
					return DataComp->DisplayMagnitude.GetValueAtLevel(InLevel);
				}

				//if(GE->Modifiers.IsValidIndex(EffectDisplayMagnitudeIndex.MagnitudeIndex))
				//{
				//	float outMag;
				//	if (GE->Modifiers[EffectDisplayMagnitudeIndex.MagnitudeIndex].ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, outMag))
				//	{
				//		return outMag;
				//	}
				//}
			}
		}
	}

	return DefaultDisplayMagnitude;
}
