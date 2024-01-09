// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "HAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	UHAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer);

	//~UAbilitySystemGlobals interface
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	//~End of UAbilitySystemGlobals interface


};
