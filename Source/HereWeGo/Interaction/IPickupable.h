#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "UObject/Interface.h"

#include "UObject/ObjectPtr.h"
#include "IPickupable.generated.h"

template <typename InterfaceType> class TScriptInterface;

class AActor;
class UHItemDefinition;
class UHInventoryItemInstance;
class UHInventoryComponent;
class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct FPickupTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StackCount = 1;

	//TODO: MAKE this into a primary asset id
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UHItemDefinition> ItemDef;
};

USTRUCT(BlueprintType)
struct FPickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UHInventoryItemInstance> Item = nullptr;
};

USTRUCT(BlueprintType)
struct FInventoryPickup
{
	GENERATED_BODY()

	FInventoryPickup()
	{
	}

	FInventoryPickup(UHInventoryItemInstance* InInstance)
	{
		if(InInstance)
		{
			FPickupInstance Pickup = FPickupInstance(InInstance);
			Instances.Add(Pickup);
		}
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPickupInstance> Instances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPickupTemplate> Templates;
};

/**  */
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

/**  */
class IPickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FInventoryPickup GetPickupInventory() const = 0;
};

/**  */
UCLASS()
class UPickupableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UPickupableStatics();

public:
	UFUNCTION(BlueprintPure)
	static TScriptInterface<IPickupable> GetFirstPickupableFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "Ability"))
	static void AddPickupToInventory(UHInventoryComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "Ability"))
	static void PushPickupToPlayer(APawn* PlayerPawn, TScriptInterface<IPickupable> Pickup);

private:
	//TODO finish async loading
	void OnWeaponLoad();
};
