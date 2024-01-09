// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Components/ActorComponent.h"
#include "HGameplayAbility.h"
#include "HWeaponComponent.generated.h"


class UHInventoryItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHMeleeAttackDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHPlayHitMarker);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHOnAttackHitEvent, TArray<FHitResult>&, Hit, UPrimitiveComponent*, WeaponMesh);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FHOnMeleeHitSignature,
	class AActor*, HitActor, class UPrimitiveComponent*, HitComponent, const FVector&, ImpactPoint, const FVector&, ImpactNormal,
	const FGameplayTag&, WeaponPartTag, FName, HitBoneName, const struct FHitResult&, HitResult);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEREWEGO_API UHWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHWeaponComponent();

	void RegisterWithAbilitySystem(UAbilitySystemComponent* ASC);

	void UnregisterWithAbilitySystem();

protected:

	void ActuallyUpdateDamageInstigatedTime();

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UHInventoryItemInstance>> EquippedItems;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UHInventoryItemInstance> CurrentEquippedItem;

	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponentRef;

public:	

	UFUNCTION(BlueprintCallable, Category = "Weapon|Abilities")
	bool CanAttack() const;

	UFUNCTION(Client, Reliable)
	void ClientConfirmTargetData(uint16 UniqueId, bool bSuccess, const TArray<uint8>& HitReplaces);

	void AddUnconfirmedServerSideHitMarkers(const FGameplayAbilityTargetDataHandle& InTargetData, const TArray<FHitResult>& FoundHits);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Melee")
	void HandleOnMeleeHitEvent(AActor* HitActor, UPrimitiveComponent* HitComponent, const FVector& ImpactPoint, const FVector& ImpactNormal, const FGameplayTag& WeaponPartTag, FName HitBoneName, const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Melee|Trace")
	void StartMeleeTrace();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Melee|Trace")
	void StopMeleeTrace();

	UPROPERTY(BlueprintReadWrite)
	float TimeOfLastAttack;

	UPROPERTY(BlueprintReadWrite)
	float AttackCooldownTime;

	UPROPERTY(BlueprintAssignable, Category = "Melee Weapon|Event")
	FHOnMeleeHitSignature OnMeleeHitTaggedEvent;

public:

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Melee|Events")
	FHMeleeAttackDelegate OnMeleeTraceStart;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Melee|Events")
	FHMeleeAttackDelegate OnMeleeTraceStop;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Melee|Events")
	FHOnAttackHitEvent OnMeleeHitFound;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FHPlayHitMarker PlayHitMarkerDelegate;

protected:

	UPROPERTY()
	TArray<uint8> UnconfirmedMeleeHitMarkers;

private:
	
};
