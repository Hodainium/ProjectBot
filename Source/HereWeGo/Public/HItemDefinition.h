// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "HItemDefinition.generated.h"



class UHInventoryItemInstance;
class UHEquipmentDefinition;
/**
 * 
 */

//////////////////////////////////////////////////////////////////////

//IF YOU CHANGE ANY OF THESE NEED TO CHANGE GETENUMASSETTYPE FUNC IN THIS CLASS
//ALSO NEED TO ADD/REMOVE STATIC ASSETTYPES IN HASSETMANAGER
//NEED TO OVERRIDE ENUM IN NEW ASSET TYPE

//Enum item type that is mapped to PrimaryAssetType
UENUM(BlueprintType)
enum EHItemType
{
	Weapon,
	Armor,
	Resource,
	ItemMod,
	CharacterMod,
	UndefinedType
};

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class HEREWEGO_API UHInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UHInventoryItemInstance* Instance) const {}
};

/**
 * ULyraInventoryItemDefinition
 */

UCLASS(Abstract, BlueprintType)
class HEREWEGO_API UHItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UHItemDefinition()
		: ItemQuality(0), Price(0)
		  , IsStackable(false), MaxStack(1)
		  , MaxDurability(0), InventorySizeX(1)
		  , InventorySizeY(1)
		  , Weight(0), MaxLevel(1)
		  , AbilityLevel(1)
	{
		ItemType = EHItemType::UndefinedType;
	}

	/** Type of this item, set in native parent class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item, AssetRegistrySearchable)
	TEnumAsByte<EHItemType> ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Item)
	FGameplayTagContainer FlavorTags;

	/** User-visible short name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemName;

	/** User-visible long description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FText ItemDescription;

	/** Icon to display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	TSoftObjectPtr<UTexture2D> ItemIcon;

	/** Item Quality */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	uint8 ItemQuality;

	/** Price in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 Price;

	/** Can this item stack? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	bool IsStackable;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxStack;

	/** Maximum number of instances that can be in inventory at once, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Durability)
	float MaxDurability;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Size")
	int InventorySizeX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Size")
	int InventorySizeY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Size")
	float Weight;

	/** Returns if the item is consumable (MaxStack <= 0)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
	bool IsConsumable() const;

	/** Maximum level this item can be, <= 0 means infinite */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
	int32 MaxLevel;

	/** Ability level this item grants. <= 0 means the character level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	int32 AbilityLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Fragments")
	TArray<TObjectPtr<UHInventoryItemFragment>> Fragments;

	/** Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable, Category = Item)
	FString GetIdentifierString() const;

	/** Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	const UHInventoryItemFragment* FindFragmentByClass(TSubclassOf<UHInventoryItemFragment> FragmentClass) const;

private:
	static const FPrimaryAssetType* GetEnumAssetType(EHItemType Enum);
};

UCLASS()
class UHInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = FragmentClass))
	static const UHInventoryItemFragment* FindItemDefinitionFragment(UHItemDefinition* ItemDef, TSubclassOf<UHInventoryItemFragment> FragmentClass);
};
