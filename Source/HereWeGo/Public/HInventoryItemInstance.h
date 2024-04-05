// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HGameplayTagStackContainer.h"
#include "HInventoryComponent.h"
#include "UObject/NoExportTypes.h"
#include "HInventoryItemInstance.generated.h"

enum class EHLootQuality : uint8;
enum EHItemType : int;
class UHItemModInstance;
class UHItemDefinition;
class UHGameplayAbility;
class UHEquipmentDefinition;
class UHInventoryItemFragment;
class AHWeaponBase;

DECLARE_DYNAMIC_DELEGATE_OneParam(FHOnItemMeshLoaded, UStaticMesh*, StaticMeshHQ);

/**
 * 
 */
UCLASS(BlueprintType)
class HEREWEGO_API UHInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:

	UHInventoryItemInstance(const FObjectInitializer& ObjectInitializer);

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag count is below 0 after it removes it
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	UHItemDefinition* GetItemDefinition() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TArray<UHItemModInstance*> GetItemMods() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	EHItemType GetItemTypeEnum() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	FText GetItemName() const;

	UFUNCTION(BlueprintCallable, Category = "Item|Display|World")
	UStaticMesh* GetDisplayMesh() const;

	//Todo implement fsyncmix and have this callback to requesting item when hq loaded
	UFUNCTION(BlueprintCallable, Category = "Item|Display|World", meta = (AutoCreateRefTerm = "Delegate"))
	UStaticMesh* GetDisplayMeshHQ(const FHOnItemMeshLoaded& Delegate) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetMaxStack() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	EHLootQuality GetItemQuality() const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool GetCanBeStacked();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	TSoftObjectPtr<UTexture2D> GetItemIcon();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool IsItemStackCompatible(UHInventoryItemInstance* IncomingItem) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UHInventoryItemFragment* FindFragmentByClass(TSubclassOf<UHInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:
	void SetItemDef(UHItemDefinition* InDef);

	void SetItemQuality(EHLootQuality InQuality);

	void AddItemMod(UHItemModInstance* InMod);

	UFUNCTION()
	void OnRep_ReplicatedAdjectiveIndexKey();

	friend struct FHInventoryList;
	friend class ULootGenGameInstanceSubsystem;

protected:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UHItemModInstance>> ItemMods;

	UPROPERTY(ReplicatedUsing="OnRep_ReplicatedAdjectiveIndexKey")
	FName ReplicatedAdjectiveIndexKey;

	UPROPERTY()
	FText CachedAdjective;

	UPROPERTY(Replicated)
	EHLootQuality ItemQuality;

	UPROPERTY(Replicated)
	FHGameplayTagStackContainer StatTags;

	UPROPERTY(Replicated)
	TObjectPtr<UHItemDefinition> ItemDefinition;
};

