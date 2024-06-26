// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryItemInstance.h"

#include "HEquipmentDefinition.h"
#include "HGameplayAbility.h"
#include "HInventoryComponent.h"
#include "HItemDefinition.h"
#include "HLogChannels.h"
#include "Logging/StructuredLog.h"
#include "HereWeGo/Items/Modifiers/HItemModDefinition.h"
#include "HereWeGo/Subsystems/LootGenGameInstanceSubsystem.h"
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
	DOREPLIFETIME(ThisClass, ItemQuality);
	DOREPLIFETIME(ThisClass, ItemMods);
	DOREPLIFETIME(ThisClass, ReplicatedAdjectiveIndexKey);
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
	return ItemMods;
}

EHItemType UHInventoryItemInstance::GetItemTypeEnum() const
{
	return ItemDefinition->ItemType;
}

FText UHInventoryItemInstance::GetItemName() const
{
	if (ItemDefinition)
	{
		if(!CachedAdjective.IsEmpty())
		{
			return FText::Format(NSLOCTEXT("WeaponUI", "WeaponTitle", "{0} {1}"), CachedAdjective, ItemDefinition->ItemName);
		}

		return ItemDefinition->ItemName;
	}

	return FText::GetEmpty();
}

float UHInventoryItemInstance::GetItemWeight() const
{
	if (ItemDefinition)
	{
		return ItemDefinition->Weight;
	}

	return 0.f;
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

EHItemQuality UHInventoryItemInstance::GetItemQuality() const
{
	return ItemQuality;
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

void UHInventoryItemInstance::SetItemQuality(EHItemQuality InQuality)
{
	ItemQuality = InQuality;
}

void UHInventoryItemInstance::SetItemAdjectiveText(FName NameKey)
{
	ReplicatedAdjectiveIndexKey = NameKey;
	OnRep_ReplicatedAdjectiveIndexKey();
}

void UHInventoryItemInstance::AddItemMod(UHItemModInstance* InMod)
{
	ItemMods.Add(InMod);
}

void UHInventoryItemInstance::OnRep_ReplicatedAdjectiveIndexKey()
{
	if(ULootGenGameInstanceSubsystem* LootSystem = GetWorld()->GetGameInstance()->GetSubsystem<ULootGenGameInstanceSubsystem>())
	{
		CachedAdjective = LootSystem->GetAdjectiveForKey(ReplicatedAdjectiveIndexKey);
		//TODO: Broadcast text changed maybe
	}
}

