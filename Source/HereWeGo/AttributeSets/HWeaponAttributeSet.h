// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "HAttributeSet.h"
#include "HWeaponAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHWeaponAttributeSet : public UHAttributeSet
{
	GENERATED_BODY()

public:

	UHWeaponAttributeSet();

	ATTRIBUTE_ACCESSORS(UHWeaponAttributeSet, WeaponDamage);
	ATTRIBUTE_ACCESSORS(UHWeaponAttributeSet, WeaponFireRate);
	ATTRIBUTE_ACCESSORS(UHWeaponAttributeSet, WeaponRange);
	ATTRIBUTE_ACCESSORS(UHWeaponAttributeSet, WeaponAccuracy);
	ATTRIBUTE_ACCESSORS(UHWeaponAttributeSet, WeaponReloadSpeed);

	//ATTRIBUTE_ACCESSORS(UHWeaponAttributeSet, Element);

	// Delegate when WeaponDamage changes, some information may be missing on the client
	mutable FHAttributeEvent OnWeaponDamageChanged;

	// Delegate when FireRate changes
	mutable FHAttributeEvent OnWeaponFireRateChanged;

	// Delegate when range changes
	mutable FHAttributeEvent OnWeaponRangeChanged;

	// Delegate when accuracy changes, some information may be missing on the client
	mutable FHAttributeEvent OnWeaponAccuracyChanged;

	// Delegate when accuracy changes, some information may be missing on the client
	mutable FHAttributeEvent OnWeaponReloadSpeedChanged;

protected:

	UFUNCTION()
	void OnRep_WeaponDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_WeaponFireRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_WeaponRange(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_WeaponAccuracy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_WeaponReloadSpeed(const FGameplayAttributeData& OldValue);


private:

	// The current damage attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponDamage, Category = "H|WeaponDamage", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData WeaponDamage;

	// The current firerate attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponFireRate, Category = "H|WeaponFireRate", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData WeaponFireRate;

	// The current range attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponRange, Category = "H|WeaponRange", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData WeaponRange;

	// The current accuracy attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponAccuracy, Category = "H|WeaponAccuracy", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData WeaponAccuracy;

	// The current accuracy attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponReloadSpeed, Category = "H|WeaponReload", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData WeaponReloadSpeed;
};