// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "IInteractableTarget.h"
#include "InteractionOption.h"
#include "IPickupable.h"
#include "HereWeGo/WorldObjects/HWorldInteractable.h"
#include "HWorldCollectable.generated.h"

class UObject;
struct FInteractionQuery;

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class AHWorldCollectable : public AHWorldInteractable, public IPickupable
{
	GENERATED_BODY()

public:

	AHWorldCollectable();

	virtual FInventoryPickup GetPickupInventory() const override;

protected:

	UPROPERTY(EditAnywhere)
	FInventoryPickup StaticInventory;
};
