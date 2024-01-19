// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HInventoryComponent.h"
#include "Components/ActorComponent.h"
#include "HGridInventoryComponent.generated.h"


class UHGridArray;
struct FHInventoryPoint;
class UHGridEntry;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEREWEGO_API UHGridInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHGridInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanStackItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);*/

	UFUNCTION(BlueprintCallable)
	bool CanStackEntries(UHGridEntry* BaseItem, UHGridEntry* RequestingItem);

	UFUNCTION(BlueprintCallable)
	int32 StackEntries(UHGridEntry* BaseItem, UHGridEntry* RequestingItem);

	UFUNCTION(BlueprintCallable)
	bool GetAllBlockingEntriesAtPointForEntry(const FHInventoryPoint& Point, UHGridEntry* RequestingEntry, TArray<UHGridEntry*> OutBlockingEntries);

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

	//Returns number of remaining items that were not added
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool TryAddItemDefinition(UHItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	int32 TryAddItemInstance(UHInventoryItemInstance* ItemInstance, int32 StackCount = 1);

private:

	UPROPERTY(Replicated) 
	FHInventoryList InventoryList;

	UPROPERTY()
	TObjectPtr<UHGridArray> LocalGridArray;

	//Because we are working with fastarray we need to handle the order that the onrep functions are called.
	//We cache PendingItemsToMove in the postadd and postchange functions and finally add them in the last called function postRep
	UPROPERTY(Transient)
	TArray<TObjectPtr<UHGridEntry>> LocalPendingItemsToMove;


	//Current inventory size. Will be replicated
	UPROPERTY(Replicated, ReplicatedUsing = "OnRep_InventorySize") 
	FHInventoryPoint InventorySize;
};
