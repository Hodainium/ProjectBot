// Fill out your copyright notice in the Description page of Project Settings.


#include "HEquipmentInstance.h"
#include "HEquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

UHEquipmentInstance::UHEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* UHEquipmentInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}

	return nullptr;
}

void UHEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

void UHEquipmentInstance::OnRep_Instigator()
{
}

APawn* UHEquipmentInstance::GetPawn() const
{
	UObject* Outer = GetOuter();

	if(APlayerState* PS = Cast<APlayerState>(Outer))
	{
		return PS->GetPawn();
	}
	else
	{
		return Cast<APawn>(GetOuter());
	}
}

APawn* UHEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if(UClass* ActualPawnType = PawnType)
	{
		if(APawn* Pawn = GetPawn())
		{
			if (Pawn->IsA(ActualPawnType))
			{
				Result = Pawn;
			}
		}
	}
	return Result;
}

void UHEquipmentInstance::SpawnEquipmentActors(const TArray<FHItemActorToSpawn>& ActorsToSpawn)
{
	if(APawn* OwningPawn = GetPawn())
	{
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Char->GetMesh();
		}

		for (const FHItemActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);
			

			SpawnedActors.Add(NewActor);
		}
	}
}

void UHEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if(Actor)
		{
			Actor->Destroy();
		}
	}
}

void UHEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UHEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}
