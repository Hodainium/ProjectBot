// Copyright Epic Games, Inc. All Rights Reserved.

#include "HWorldCollectable.h"

#include "Async/TaskGraphInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HWorldCollectable)

struct FInteractionQuery;

AHWorldCollectable::AHWorldCollectable()
{
}

FInventoryPickup AHWorldCollectable::GetPickupInventory() const
{
	return StaticInventory;
}
