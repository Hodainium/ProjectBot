// Copyright Epic Games, Inc. All Rights Reserved.

#include "HWorldCollectable.h"

#include "Async/TaskGraphInterfaces.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HWorldCollectable)

struct FInteractionQuery;

AHWorldCollectable::AHWorldCollectable()
{
}

FInventoryPickup AHWorldCollectable::GetPickupInventory() const
{
	return StaticInventory;
}

AHWorldCollectableInstance::AHWorldCollectableInstance()
{
	bReplicates = true;
}

FInventoryPickup AHWorldCollectableInstance::GetPickupInventory() const
{
	if(ItemInstance)
	{
		return FInventoryPickup(ItemInstance);
	}

	return FInventoryPickup();
}

void AHWorldCollectableInstance::OnItemInstanceSet_Implementation()
{
}

void AHWorldCollectableInstance::OnRep_ItemInstance()
{
	OnItemInstanceSet();
}

void AHWorldCollectableInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHWorldCollectableInstance, ItemInstance);
}
