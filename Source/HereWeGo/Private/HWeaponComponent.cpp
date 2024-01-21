// Fill out your copyright notice in the Description page of Project Settings.


#include "HWeaponComponent.h"

#include "AbilitySystemComponent.h"
#include "HEquipmentDefinition.h"
#include "HInventoryItemInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include <HCharacterBase.h>

// Sets default values for this component's properties
UHWeaponComponent::UHWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UHWeaponComponent::ActuallyUpdateDamageInstigatedTime()
{
	// If our LastWeaponDamageInstigatedTime was not very recent, clear our LastWeaponDamageScreenLocations array
	UWorld* World = GetWorld();
	if (World->GetTimeSeconds() - TimeOfLastAttack > 0.1)
	{
		//LastWeaponDamageScreenLocations.Reset();
	}
	TimeOfLastAttack = World->GetTimeSeconds();
}

void UHWeaponComponent::RegisterWithAbilitySystem(UAbilitySystemComponent* ASC)
{

}

void UHWeaponComponent::UnregisterWithAbilitySystem()
{
}

bool UHWeaponComponent::CanAttack() const
{
	if(AttackCooldownTime <= GetWorld()->TimeSeconds - TimeOfLastAttack)
	{
		return true;
	}

	return false;
}

void UHWeaponComponent::AddUnconfirmedServerSideHitMarkers(const FGameplayAbilityTargetDataHandle& InTargetData,
                                                           const TArray<FHitResult>& FoundHits)
{
	UnconfirmedMeleeHitMarkers.Emplace(InTargetData.UniqueId);
}

void UHWeaponComponent::HandleOnMeleeHitEvent(AActor* HitActor, UPrimitiveComponent* HitComponent,
	const FVector& ImpactPoint, const FVector& ImpactNormal, const FGameplayTag& WeaponPartTag, FName HitBoneName,
	const FHitResult& HitResult)
{
	if(OnMeleeHitTaggedEvent.IsBound())
	{
		OnMeleeHitTaggedEvent.Broadcast(HitActor, HitComponent, ImpactPoint, ImpactNormal, WeaponPartTag, HitBoneName, HitResult);
	}
	//Do more here?

	if (AActor* HitActorLocal = HitResult.GetActor()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("HitActor named: %s"), *HitActorLocal->GetName());

		AHCharacterBase* CharBase = Cast<AHCharacterBase>(HitActorLocal);

		if (CharBase)
		{
			UAbilitySystemComponent* TargetASC = CharBase->GetAbilitySystemComponent();

			if (TargetASC)
			{
				/*ensure(DamageGameplayEffect);

				FGameplayEffectContextHandle DamageContextHandle = AbilitySystemComponentRef->MakeEffectContext();
				DamageContextHandle.AddHitResult(HitResult);

				FGameplayEffectSpecHandle DamageHandle = AbilitySystemComponentRef->MakeOutgoingSpec(DamageGameplayEffect, AttributeSetBaseRef->GetLevel(), DamageContextHandle);
				DamageHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), GunDamage);

				if (DamageHandle.IsValid())
				{
					FActiveGameplayEffectHandle AppliedEffect = AbilitySystemComponentRef->ApplyGameplayEffectSpecToTarget(*DamageHandle.Data.Get(), TargetASC);
				}*/
			}
		}
	}
}

void UHWeaponComponent::StartMeleeTrace()
{
	OnMeleeTraceStart.Broadcast();
}

void UHWeaponComponent::StopMeleeTrace()
{
	OnMeleeTraceStop.Broadcast();
}

void UHWeaponComponent::ClientConfirmTargetData_Implementation(uint16 UniqueId, bool bSuccess, const TArray<uint8>& HitReplaces)
{
	for (int i = 0; i < UnconfirmedMeleeHitMarkers.Num(); i++)
	{
		if (UnconfirmedMeleeHitMarkers[i] == UniqueId)
		{
			if (bSuccess)
			{
				UWorld* World = GetWorld();
				bool bFoundShowAsSuccessHit = false;
				
				// Only need to do this once
				if (!bFoundShowAsSuccessHit)
				{
					ActuallyUpdateDamageInstigatedTime();
				}

				bFoundShowAsSuccessHit = true;

				//LastWeaponDamageScreenLocations.Add(Entry);
				PlayHitMarkerDelegate.Broadcast();
			}

			UnconfirmedMeleeHitMarkers.RemoveAt(i);
			break;
		}
	}
}




