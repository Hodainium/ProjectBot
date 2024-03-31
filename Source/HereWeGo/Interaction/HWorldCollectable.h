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

UCLASS(Abstract, Blueprintable)
class AHWorldCollectableInstance : public AHWorldInteractable, public IPickupable
{
	GENERATED_BODY()

public:

	AHWorldCollectableInstance();

	virtual FInventoryPickup GetPickupInventory() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnItemInstanceSet();

	UFUNCTION()
	void OnRep_ItemInstance();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = "OnRep_ItemInstance", meta = (ExposeOnSpawn = true))
	TObjectPtr<UHInventoryItemInstance> ItemInstance = nullptr;
};
