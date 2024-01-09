// Fill out your copyright notice in the Description page of Project Settings.


#include "HWeaponBaseMelee.h"
#include "HWeaponComponent.h"
#include "HWeaponInterface.h"
#include "RamaMeleeWeapon.h"

AHWeaponBaseMelee::AHWeaponBaseMelee(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MeleeComponent = CreateDefaultSubobject<URamaMeleeWeapon>(TEXT("MeleeComp"));
	SetRootComponent(MeleeComponent);
}

void AHWeaponBaseMelee::BeginPlay()
{
	Super::BeginPlay();

	if(const IHWeaponInterface* WeaponInterface = Cast<IHWeaponInterface>(GetOwner()))
	{
		OwningWeaponComponentRef = WeaponInterface->GetWeaponComponent();

		if(OwningWeaponComponentRef.IsValid())
		{
			OwningWeaponComponentRef->OnMeleeTraceStart.AddDynamic(MeleeComponent, &URamaMeleeWeapon::StartSwingDamage);
			OwningWeaponComponentRef->OnMeleeTraceStop.AddDynamic(MeleeComponent, &URamaMeleeWeapon::StopSwingDamage);

			MeleeComponent->RamaMeleeWeapon_OnHit.AddDynamic(this, &ThisClass::HandleOnRamaMeleeHitEvent);
		}

		
		//Need to bind to weaponComp
	}
	
	
}

void AHWeaponBaseMelee::HandleOnRamaMeleeHitEvent(AActor* HitActor, UPrimitiveComponent* HitComponent,
	const FVector& ImpactPoint, const FVector& ImpactNormal, int32 ShapeIndex, FName HitBoneName,
	const FHitResult& HitResult)
{
	if (ShapeIndex >= 0 && DamageMapShapeTags.Num() > 0)
	{
		if(const FGameplayTag* PartTag = DamageMapShapeTags.Find(ShapeIndex))
		{
			if(OwningWeaponComponentRef.IsValid())
			{
				OwningWeaponComponentRef->HandleOnMeleeHitEvent(HitActor, HitComponent, ImpactPoint, ImpactNormal, *PartTag, HitBoneName, HitResult);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid shape index in weaponmeleebase: %s"), *GetName())
	}
}
