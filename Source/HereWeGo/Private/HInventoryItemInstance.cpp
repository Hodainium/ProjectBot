// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryItemInstance.h"

#include "HEquipmentDefinition.h"
#include "HGameplayAbility.h"
#include "HInventoryComponent.h"
#include "HItemDefinition.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"
#include "HereWeGo/Items/Modifiers/HItemModDefinition.h"
#include "Net/UnrealNetwork.h"

UHInventoryItemInstance::UHInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UHInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDefinition);
	DOREPLIFETIME(ThisClass, ItemQualityTag);
	DOREPLIFETIME(ThisClass, Mods);
}

void UHInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UHInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 UHInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool UHInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

UHItemDefinition* UHInventoryItemInstance::GetItemDefinition() const
{
	return ItemDefinition;
}

TArray<UHItemModInstance*> UHInventoryItemInstance::GetItemMods() const
{
	return Mods;
}

EHItemType UHInventoryItemInstance::GetItemTypeEnum() const
{
	return ItemDefinition->ItemType;
}

UStaticMesh* UHInventoryItemInstance::GetDisplayMesh() const
{
	if(ItemDefinition)
	{
		return ItemDefinition->LowQualityWorldModel;
	}

	return nullptr;
}

UStaticMesh* UHInventoryItemInstance::GetDisplayMeshHQ(const FHOnItemMeshLoaded& Delegate) const
{
	if (ItemDefinition)
	{
		if(UStaticMesh* HQMesh = ItemDefinition->HighQualityWorldModel.Get())
		{
			return HQMesh;
		}
		//todo start async load here and link callback
		return ItemDefinition->LowQualityWorldModel;
	}


	return nullptr;
}

int32 UHInventoryItemInstance::GetMaxStack() const
{
	if(ItemDefinition)
	{
		return ItemDefinition->MaxStack;
	}

	return INDEX_NONE;
}

FGameplayTag UHInventoryItemInstance::GetItemQuality() const
{
	return ItemQualityTag;
}

bool UHInventoryItemInstance::GetCanBeStacked()
{
	return ItemDefinition->CanBeStacked;
}

TSoftObjectPtr<UTexture2D> UHInventoryItemInstance::GetItemIcon()
{
	if (ItemDefinition)
	{
		return ItemDefinition->ItemIcon;
	}

	return nullptr;
}

bool UHInventoryItemInstance::IsItemStackCompatible(UHInventoryItemInstance* IncomingItem) const
{
	if (UHItemDefinition* IncomingItemDef = IncomingItem->GetItemDefinition())
	{
		if(ItemDefinition == IncomingItemDef)
		{
			UE_LOGFMT(LogHGame, Warning, "INVENTORY: Can stack item");
			return true;
		}
	}

	UE_LOGFMT(LogHGame, Warning, "INVENTORY: Cannot stack item");

	return false;
}

const UHInventoryItemFragment* UHInventoryItemInstance::FindFragmentByClass(
	TSubclassOf<UHInventoryItemFragment> FragmentClass) const
{
	if ((ItemDefinition != nullptr) && (FragmentClass != nullptr))
	{
		return ItemDefinition->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

void UHInventoryItemInstance::SetItemDef(UHItemDefinition* InDef)
{
	ItemDefinition = InDef;
}

