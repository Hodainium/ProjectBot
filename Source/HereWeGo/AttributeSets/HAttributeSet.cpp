// Fill out your copyright notice in the Description page of Project Settings.


#include "HAttributeSet.h"

#include "HAbilitySystemComponent.h"


class UWorld;


UHAttributeSet::UHAttributeSet()
{
}

UWorld* UHAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UHAbilitySystemComponent* UHAttributeSet::GetHAbilitySystemComponent() const
{
	return Cast<UHAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

