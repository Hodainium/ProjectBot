// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "Components/ActorComponent.h"
#include "HGridInventoryComponent.generated.h"

class UHGridArray;
struct FHInventoryOperation;
class UHInventoryItemInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEREWEGO_API UHGridInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHGridInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanStackItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	UHInventoryItemInstance* AddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddItemInstance(UHInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveItemInstance(UHInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure = false)
	TArray<UHInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
	UHInventoryItemInstance* FindFirstItemStackByDefinition(UHItemDefinition* ItemDef) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int32 GetTotalItemCountByDefinition(UHItemDefinition* ItemDef) const;
	bool ConsumeItemsByDefinition(UHItemDefinition* ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	///////// For blueprint

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory Functions")
	void SetInventorySize(int32 Width, int32 Height);

private:

	

	//OnRep function for inventory size. Unimplemented right now but should resize grid and reposition items
	//Will get called in SP as well. Should resize grid array and reorganize items here
	//Will need to eventually change positions of struct vars if on server or maybe server should have separate function
	UFUNCTION()
	void OnRep_InventorySize();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool TryAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool TryAddItemInstance(UHInventoryItemInstance* ItemInstance, int32 StackCount = 1);

private:

	UPROPERTY(Replicated)
	FHInventoryList InventoryList;

	UPROPERTY()
	TObjectPtr<UHGridArray> LocalGridArray;

	//Because we are working with fastarray we need to handle the order that the onrep functions are called.
	//We cache PendingItemsToMove in the postadd and postchange functions and finally add them in the last called function postRep
	UPROPERTY(Transient)
	TArray<TObjectPtr<UHGridItem>> LocalPendingItemsToMove;

	//This will be overlaid on top of a constructed grid. The plan is to send these to server, server responds with same key and confirms it or not.
	//May return a correction rather than just outright denying. So user will be able to predict in case server sends an item in the occupying slot.
	//Server always overrides predicted items and sends to "Next best slot" if no next slot is available we drop item.
	//Will that be another prediction or linked to awaiting prediction key?
	UPROPERTY(Transient)
	TArray<FHInventoryOperation> PredictedOperations;

	//Current inventory size. Will be replicated
	UPROPERTY(Replicated, ReplicatedUsing="OnRep_InventorySize")
	FHInventoryPoint InventorySize;
};
