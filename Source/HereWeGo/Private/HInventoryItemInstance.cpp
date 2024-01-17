// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryItemInstance.h"

#include "HEquipmentDefinition.h"
#include "HGameplayAbility.h"
#include "HInventoryComponent.h"
#include "HItemDefinition.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"
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

int32 UHInventoryItemInstance::GetMaxStack() const
{
	if(ItemDefinition)
	{
		return ItemDefinition->MaxStack;
	}

	return INDEX_NONE;
}

bool UHInventoryItemInstance::GetCanBeStacked()
{
	return ItemDefinition->CanBeStacked;
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

FHInventoryPoint UHInventoryItemInstance::GetItemDimensions() const
{
	return FHInventoryPoint(ItemDefinition->InventorySizeX, ItemDefinition->InventorySizeY);
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

