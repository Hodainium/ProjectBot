// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HAttributeSet.h"
#include "HHealthSet.generated.h"

struct FGameplayEffectModCallbackData;


/**
 * UHHealthSet
 *
 *	Class that defines attributes that are necessary for taking damage.
 *	Attribute examples include: health, shields, and resistances.
 */
UCLASS(BlueprintType)
class HEREWEGO_API UHHealthSet : public UHAttributeSet
{
	GENERATED_BODY()

public:

	UHHealthSet();

	ATTRIBUTE_ACCESSORS(UHHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UHHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UHHealthSet, Shield);
	ATTRIBUTE_ACCESSORS(UHHealthSet, MaxShield);
	ATTRIBUTE_ACCESSORS(UHHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UHHealthSet, Damage);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FHAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FHAttributeEvent OnMaxHealthChanged;

	// Delegate when shield changes due to damage/healing, some information may be missing on the client
	mutable FHAttributeEvent OnShieldChanged;

	// Delegate when max shield changes
	mutable FHAttributeEvent OnMaxShieldChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FHAttributeEvent OnOutOfHealth;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FHAttributeEvent OnOutOfShield;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "H|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "H|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// The current shield attribute.  The health will be capped by the max shield attribute.  Shield is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "H|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Shield;

	// The current max shield attribute.  Max shield is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxShield, Category = "H|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxShield;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	// Used to track when the shield reaches 0.
	bool bOutOfShield;

	// Store the health before any changes 
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	float MaxShieldBeforeAttributeChange;
	float ShieldBeforeAttributeChange;

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Category="H|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category="H|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};