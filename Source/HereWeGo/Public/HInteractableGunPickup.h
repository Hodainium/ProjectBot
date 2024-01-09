// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "HInteractableGunPickup.generated.h"

UCLASS()
class HEREWEGO_API AHInteractableGunPickup : public AActor, public IHGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHInteractableGunPickup();

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

	virtual bool OnInteract_Implementation(APawn* InstigatorPawn) override;

	UPROPERTY(EditDefaultsOnly)
	FText GunName;

	UPROPERTY(EditDefaultsOnly)
	float FireRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float ShootTraceDistance;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> GunMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
