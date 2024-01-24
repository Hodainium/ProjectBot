// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
//#include "HBaseGameplayAbility.generated.h"

class AHPlayerCharacter;
class UHAbilityCost;
class IHAbilitySourceInterface;
class UHCameraMode;
class AHPlayerController;
class AHCharacterBaseOld;
class UHAbilitySystemComponent;

//UENUM(BlueprintType)
//enum class EHAbilityActivationPolicy : uint8
//{
//	// Try to activate the ability when the input is triggered.
//	OnInputTriggered,
//
//	// Continually try to activate the ability while the input is active.
//	WhileInputActive,
//
//	// Try to activate the ability when an avatar is assigned.
//	OnSpawn
//};
//
//UENUM(BlueprintType)
//enum class EHAbilityActivationGroup : uint8
//{
//	// Ability runs independently of all other abilities.
//	Independent,
//
//	// Ability is canceled and replaced by other exclusive abilities.
//	Exclusive_Replaceable,
//
//	// Ability blocks all other exclusive abilities from activating.
//	Exclusive_Blocking,
//
//	MAX	UMETA(Hidden)
//};

/**
 * 
 */
//UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
//class HEREWEGO_API UHBaseGameplayAbility : public UGameplayAbility
//{
//	GENERATED_BODY()
//
//	friend class UHAbilitySystemComponent;
//
//public:
//
//	UHBaseGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	UFUNCTION(BlueprintCallable, Category = "H|Ability")
//	UHAbilitySystemComponent* GetHAbilitySystemComponentFromActorInfo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "H|Ability")
//	AHPlayerController* GetHPlayerControllerFromActorInfo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "H|Ability")
//	AController* GetControllerFromActorInfo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "H|Ability")
//	AHCharacterBaseOld* GetHCharacterBaseFromActorInfo() const;
//
//	UFUNCTION(BlueprintCallable, Category = "H|Ability")
//	AHPlayerCharacter* GetHPlayerCharacterFromActorInfo() const;
//
//	EHAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
//	EHAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }
//
//	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
//
//	// Returns true if the requested activation group is a valid transition.
//	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "H|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
//	bool CanChangeActivationGroup(EHAbilityActivationGroup NewGroup) const;
//
//	// Tries to change the activation group.  Returns true if it successfully changed.
//	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "H|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
//	bool ChangeActivationGroup(EHAbilityActivationGroup NewGroup);
//
//	// Sets the ability's camera mode.
//	UFUNCTION(BlueprintCallable, Category = "H|Ability")
//	void SetCameraMode(TSubclassOf<UHCameraMode> CameraMode);
//
//	// Clears the ability's camera mode.  Automatically called if needed when the ability ends.
//	UFUNCTION(BlueprintCallable, Category = "H|Ability")
//	void ClearCameraMode();
//
//	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
//	{
//		NativeOnAbilityFailedToActivate(FailedReason);
//		ScriptOnAbilityFailedToActivate(FailedReason);
//	}
//
//protected:
//
//	// Called when the ability fails to activate
//	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
//
//	// Called when the ability fails to activate
//	UFUNCTION(BlueprintImplementableEvent)
//	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
//
//	//~UGameplayAbility interface
//	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
//	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
//	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
//	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
//	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
//	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
//	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
//	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
//	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
//	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
//	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
//	//~End of UGameplayAbility interface
//
//	virtual void OnPawnAvatarSet();
//
//	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IHAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const;
//
//	/** Called when this ability is granted to the ability system component. */
//	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
//	void K2_OnAbilityAdded();
//
//	/** Called when this ability is removed from the ability system component. */
//	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
//	void K2_OnAbilityRemoved();
//
//	/** Called when the ability system is initialized with a pawn avatar. */
//	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
//	void K2_OnPawnAvatarSet();
//
//protected:
//
//	// Defines how this ability is meant to activate.
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "H|Ability Activation")
//	EHAbilityActivationPolicy ActivationPolicy;
//
//	// Defines the relationship between this ability activating and other abilities activating.
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "H|Ability Activation")
//	EHAbilityActivationGroup ActivationGroup;
//
//	// Additional costs that must be paid to activate this ability
//	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs)
//	TArray<TObjectPtr<UHAbilityCost>> AdditionalCosts;
//
//	// Map of failure tags to simple error messages
//	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
//	TMap<FGameplayTag, FText> FailureTagToUserFacingMessages;
//
//	// Map of failure tags to anim montages that should be played with them
//	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
//	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;
//
//	// If true, extra information should be logged when this ability is canceled. This is temporary, used for tracking a bug.
//	UPROPERTY(EditDefaultsOnly, Category = "Advanced")
//	bool bLogCancelation;
//
//	// Current camera mode set by the ability.
//	TSubclassOf<UHCameraMode> ActiveCameraMode;
//};
