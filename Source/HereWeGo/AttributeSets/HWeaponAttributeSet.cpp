// Fill out your copyright notice in the Description page of Project Settings.


#include "HWeaponAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UHWeaponAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHWeaponAttributeSet, WeaponDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWeaponAttributeSet, WeaponFireRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWeaponAttributeSet, WeaponRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWeaponAttributeSet, WeaponAccuracy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHWeaponAttributeSet, WeaponReloadSpeed, COND_None, REPNOTIFY_Always);
}

UHWeaponAttributeSet::UHWeaponAttributeSet()
	: WeaponDamage(0.f)
	, WeaponFireRate(0.f)
	, WeaponRange(0.f)
	, WeaponAccuracy(0.f)
	, WeaponReloadSpeed(0.f)
{
}

void UHWeaponAttributeSet::OnRep_WeaponDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHWeaponAttributeSet, WeaponDamage, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnWeaponDamageChanged.Broadcast(nullptr, nullptr, nullptr, GetWeaponDamage() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetWeaponDamage());
}

void UHWeaponAttributeSet::OnRep_WeaponFireRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHWeaponAttributeSet, WeaponFireRate, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnWeaponFireRateChanged.Broadcast(nullptr, nullptr, nullptr, GetWeaponFireRate() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetWeaponFireRate());
}

void UHWeaponAttributeSet::OnRep_WeaponRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHWeaponAttributeSet, WeaponRange, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnWeaponRangeChanged.Broadcast(nullptr, nullptr, nullptr, GetWeaponRange() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetWeaponRange());
}

void UHWeaponAttributeSet::OnRep_WeaponAccuracy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHWeaponAttributeSet, WeaponAccuracy, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnWeaponAccuracyChanged.Broadcast(nullptr, nullptr, nullptr, GetWeaponAccuracy() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetWeaponAccuracy());
}

void UHWeaponAttributeSet::OnRep_WeaponReloadSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHWeaponAttributeSet, WeaponReloadSpeed, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnWeaponReloadSpeedChanged.Broadcast(nullptr, nullptr, nullptr, GetWeaponReloadSpeed() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetWeaponReloadSpeed());
}
