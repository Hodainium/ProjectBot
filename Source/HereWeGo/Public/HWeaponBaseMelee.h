// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HWeaponBase.h"
#include "HWeaponBaseMelee.generated.h"


class UHWeaponComponent;
class URamaMeleeWeapon;

/**
 * 
 */
UCLASS()
class HEREWEGO_API AHWeaponBaseMelee : public AHWeaponBase
{
	GENERATED_BODY()

public:
	AHWeaponBaseMelee(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Rama Melee Weapon|TagMap")
	TMap<int32, FGameplayTag> DamageMapShapeTags;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<URamaMeleeWeapon> MeleeComponent;

protected:

	UPROPERTY()
	TWeakObjectPtr<UHWeaponComponent> OwningWeaponComponentRef;

	UFUNCTION()
	void HandleOnRamaMeleeHitEvent(AActor* HitActor, UPrimitiveComponent* HitComponent,
		const FVector& ImpactPoint, const FVector& ImpactNormal, int32 ShapeIndex, FName HitBoneName,
		const FHitResult& HitResult);
};
