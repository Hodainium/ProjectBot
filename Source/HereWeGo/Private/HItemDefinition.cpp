// Fill out your copyright notice in the Description page of Project Settings.


#include "HItemDefinition.h"

#include "HLogChannels.h"
#include "HereWeGo/HAssetManager.h"

FString UHItemDefinition::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

const FPrimaryAssetType* UHItemDefinition::GetEnumAssetType(EHItemType Enum)
{
	switch(Enum)
	{
		case EHItemType::Weapon:
		{
			return &UHAssetManager::WeaponItemType;
		}
		case EHItemType::Armor:
		{
			return &UHAssetManager::ArmorItemType;
		}
		case EHItemType::Resource:
		{
			return &UHAssetManager::ResourceItemType;
		}
		case EHItemType::ItemMod:
		{
			return &UHAssetManager::ItemModItemType;
		}
		case EHItemType::CharacterMod:
		{
			return &UHAssetManager::CharacterModItemType;
		}
		default:
		case EHItemType::UndefinedType:
		{
			UE_LOG(LogHGame, Error, TEXT("ItemAsset DOES NOT HAVE ENUM TO ASSETTYPE DEFINED!!!!"));
			return &UHAssetManager::UndefinedItemType;
		}
	}
}

FPrimaryAssetId UHItemDefinition::GetPrimaryAssetId() const
{
	// This is a DataAsset and not a blueprint so we can just use the raw FName
	// For blueprints you need to handle stripping the _C suffix

	return FPrimaryAssetId(*GetEnumAssetType(ItemType), GetFName());
}

const UHInventoryItemFragment* UHItemDefinition::FindFragmentByClass(
	TSubclassOf<UHInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UHInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

bool UHItemDefinition::IsConsumable() const
{
	if (MaxStack <= 0)
	{
		return true;
	}
	return false;
}

const UHInventoryItemFragment* UHInventoryFunctionLibrary::FindItemDefinitionFragment(
	UHItemDefinition* ItemDef, TSubclassOf<UHInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return ItemDef->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
