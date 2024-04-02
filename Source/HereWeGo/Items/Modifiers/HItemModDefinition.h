// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "HItemModDefinition.generated.h"

//Like fragments but 100% different. These define the base of mnodifiers we can recieve. Maybe should have their own fragments. 

enum class EHItemQuality : uint8;
class UHAbilitySystemComponent;
class UHGameplayAbility;
class UGameplayEffect;
class UHModifiedWeaponInstance;
struct FGameplayAbilitySpecHandle;
struct FActiveGameplayEffectHandle;
class UAttributeSet;
class UHItemModInstance;
class UHItemModFragment;
// DefaultToInstanced, EditInlineNew, Abstract

USTRUCT(BlueprintType)
struct FHItemModDef_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHGameplayAbility> Ability = nullptr;

	// Level of ability to grant
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;


	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


USTRUCT(BlueprintType)
struct FHItemModDef_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effct
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct FHItemModDef_GameplayCue
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FGameplayEffectCue GameplayCue;
};

USTRUCT(BlueprintType)
struct FHItemModDef_TextureOverride
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* TextureOverrides;
};

USTRUCT(BlueprintType)
struct FHItemModDef_DamageType
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageGE = nullptr;

	// Level of gameplay effct
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct FHItemModDef_ActorToSpawn
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform AttachTransform;
};

USTRUCT(BlueprintType)
struct FHItemModDef_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddGameplayCue(const FGameplayEffectCue& InCue);
	void AddGrantedDamageGE(const FHItemModDef_DamageType& Effect);
	void AddGrantedOnHitGE(const FHItemModDef_GameplayEffect& Effect);
	void AddGrantedTexture(const FHItemModDef_TextureOverride& Tex);
	void AddActorSpawned(AActor* Actor);

	void RemoveModFromEquipmentInstance(UHModifiedWeaponInstance* Instance);

protected:
	//Handles to granted abilities
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	//Handles to granted effects
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	//Granted gameplay cues
	UPROPERTY()
	FGameplayTagContainer GrantedPersistingCues;

	//Granted damageTypes
	UPROPERTY()
	TArray<FHItemModDef_DamageType> GrantedDamageTypes;

	//Granted damageTypes
	UPROPERTY()
	TArray<FHItemModDef_GameplayEffect> GrantedEffectsToApplyOnHit;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FHItemModDef_TextureOverride> GrantedTextureOverrides;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<TObjectPtr<AActor>> ExtraActorsSpawned;
};

UCLASS(BlueprintType, Const)
class HEREWEGO_API UHItemModDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UHItemModDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//Need to remove just here for now. The function will just have mag=1
	void AddModToEquipmentInstance(UHModifiedWeaponInstance* Instance, FHItemModDef_GrantedHandles* OutGrantedHandles, int Magnitude = 1) const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

public:
	/** Text that describes the mod */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText Title;

	/** Text that describes the mod */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	bool bDisplayInUI = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AssetRegistrySearchable ,Category = "Quality", meta = (Categories = "Item.Quality"))
	EHItemQuality Quality;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AssetRegistrySearchable, Category = "Tags")
	FGameplayTagContainer AssetTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AssetRegistrySearchable, Category = "Tags")
	FGameplayTagContainer RequiredTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, AssetRegistrySearchable, Category = "Tags")
	FGameplayTagContainer BlockedTags;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FHItemModDef_GameplayAbility> GameplayAbilitiesToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<FHItemModDef_GameplayEffect> GameplayEffectsToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Cues")
	TArray<FHItemModDef_GameplayCue> GameplayCuesToPlay;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<FHItemModDef_DamageType> DamageTypesToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<FHItemModDef_GameplayEffect> EffectsToApplyOnHit;

	UPROPERTY(EditDefaultsOnly, Category = "Graphical")
	TArray<FHItemModDef_TextureOverride> TextureOverrides;

	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TArray<FHItemModDef_ActorToSpawn> ActorsToSpawn;
};



