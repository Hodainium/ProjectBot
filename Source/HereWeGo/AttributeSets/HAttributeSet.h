// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "HAttributeSet.generated.h"

class AActor;
class UHAbilitySystemComponent;
class UObject;
class UWorld;
struct FGameplayEffectSpec;


/**
 * This macro defines a set of helper functions for accessing and initializing attributes.
 *
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(UHHealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

 /**
  * Delegate used to broadcast attribute events, some of these parameters may be null on clients:
  * @param EffectInstigator	The original instigating actor for this event
  * @param EffectCauser		The physical actor that caused the change
  * @param EffectSpec		The full effect spec for this change
  * @param EffectMagnitude	The raw magnitude, this is before clamping
  * @param OldValue			The value of the attribute before it was changed
  * @param NewValue			The value after it was changed
 */
DECLARE_MULTICAST_DELEGATE_SixParams(FHAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

/**
 * UHAttributeSet
 *
 *	Base attribute set class for the project.
 */
UCLASS()
class HEREWEGO_API UHAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UHAttributeSet();

	virtual UWorld* GetWorld() const override;

	UHAbilitySystemComponent* GetHAbilitySystemComponent() const;
};
