// Fill out your copyright notice in the Description page of Project Settings.


#include "HWeaponInstance.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"

UHWeaponInstance::UHWeaponInstance(const FObjectInitializer& ObjectInitializer)
{
}

void UHWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	UWorld* World = GetWorld();
	check(World);
	TimeLastEquipped = World->GetTimeSeconds();
}

void UHWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

void UHWeaponInstance::UpdateFiringTime()
{
	UWorld* World = GetWorld();
	check(World); 
	TimeLastEquipped = World->GetTimeSeconds();
}

float UHWeaponInstance::GetTimeSinceLastInteractedWith() const
{
	UWorld* World = GetWorld();
	check(World);
	const double WorldTime = World->GetTimeSeconds();

	double Result = WorldTime - TimeLastEquipped;

	if (TimeLastFired > 0.0)
	{
		const double TimeSinceFired = WorldTime - TimeLastFired;
		Result = FMath::Min(Result, TimeSinceFired);
	}

	return Result;
}

TSubclassOf<UAnimInstance> UHWeaponInstance::PickBestAnimLayer(bool bEquipped,
	const FGameplayTagContainer& CosmeticTags) const
{
	const FHAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UnequippedAnimSet);
	return SetToQuery.SelectBestLayer(CosmeticTags);
}

UHAbilitySystemComponent* UHWeaponInstance::GetASCFromOwningPawn()
{
	if(APawn* Pawn = GetPawn())
	{
		return Cast<UHAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn));
	}

	return nullptr;
}
