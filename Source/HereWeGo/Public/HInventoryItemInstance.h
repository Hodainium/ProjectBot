// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HGameplayTagStackContainer.h"
#include "HInventoryComponent.h"
#include "UObject/NoExportTypes.h"
#include "HInventoryItemInstance.generated.h"

class UHItemModInstance;
class UHItemDefinition;
class UHGameplayAbility;
class UHEquipmentDefinition;
class UHInventoryItemFragment;
class AHWeaponBase;
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
	int32 GetMaxStack() const;

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

	friend struct FHInventoryList;

protected:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UHItemModInstance>> Mods;

	UPROPERTY(Replicated)
	FHGameplayTagStackContainer StatTags;

	UPROPERTY(Replicated)
	TObjectPtr<UHItemDefinition> ItemDefinition;
};

