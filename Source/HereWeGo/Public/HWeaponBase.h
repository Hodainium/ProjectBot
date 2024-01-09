// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HWeaponBase.generated.h"


UCLASS()
class HEREWEGO_API AHWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHWeaponBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnFireEffects(const TArray<FVector>& ImpactPositions, const TArray<FVector>& ImpactNormals, EPhysicalSurface ImpactSurfaceTypes);

	virtual void OnFireEffects_Implementation(const TArray<FVector>& ImpactPositions, const TArray<FVector>& ImpactNormals, EPhysicalSurface ImpactSurfaceTypes);
};
