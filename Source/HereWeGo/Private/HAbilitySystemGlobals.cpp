// Fill out your copyright notice in the Description page of Project Settings.


#include "HAbilitySystemGlobals.h"

#include "HGameplayEffectContext.h"

UHAbilitySystemGlobals::UHAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UHAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FHGameplayEffectContext();
}
