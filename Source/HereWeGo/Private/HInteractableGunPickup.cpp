// Fill out your copyright notice in the Description page of Project Settings.


#include "HInteractableGunPickup.h"

#include "HPlayerCharacter.h"

// Sets default values
AHInteractableGunPickup::AHInteractableGunPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
	FireRate = 0.1f;
	ShootTraceDistance = 500.f;
}

FText AHInteractableGunPickup::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::FormatNamed(NSLOCTEXT("InteractUI", "PickUpGun", "Press 'E' to pick up: {GunName}"), TEXT("GunName"), GunName);
}

bool AHInteractableGunPickup::OnInteract_Implementation(APawn* InstigatorPawn)
{
	//AttachToComponent()
	if(AHPlayerCharacter* Player = Cast<AHPlayerCharacter>(InstigatorPawn))
	{
		AActor* GunActor = Cast<AActor>(this);

		if (GunActor && Player->AttemptEquipInteractWeapon(GunActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Equipped123"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("EquippedFailed"));
		}
	}



	UE_LOG(LogTemp, Warning, TEXT("Interacted with gun"));
	return true;
}

// Called when the game starts or when spawned
void AHInteractableGunPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHInteractableGunPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

