// Fill out your copyright notice in the Description page of Project Settings.


#include "HArmorInstance.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

UHArmorInstance::UHArmorInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* UHArmorInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}

	return nullptr;
}

void UHArmorInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedMeshes);
}

void UHArmorInstance::OnRep_Instigator()
{
}

APawn* UHArmorInstance::GetPawn() const
{
	UObject* Outer = GetOuter();

	if (APlayerState* PS = Cast<APlayerState>(Outer))
	{
		return PS->GetPawn();
	}
	else
	{
		return Cast<APawn>(GetOuter());
	}
}

APawn* UHArmorInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (APawn* Pawn = GetPawn())
		{
			if (Pawn->IsA(ActualPawnType))
			{
				Result = Pawn;
			}
		}
	}
	return Result;
}

void UHArmorInstance::SpawnMeshes(const TArray<FHArmorMeshToSpawn>& ActorsToSpawn)
{
	UE_LOG(LogTemp, Error, TEXT("SpawnMeshes not implemented yet!!!"))
}

void UHArmorInstance::DestroyMeshes()
{
	for (USkeletalMesh* Mesh: SpawnedMeshes)
	{
		if (Mesh)
		{
			Mesh->BeginDestroy();
		}
	}
}

void UHArmorInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UHArmorInstance::OnUnequipped()
{
	K2_OnUnequipped();
}