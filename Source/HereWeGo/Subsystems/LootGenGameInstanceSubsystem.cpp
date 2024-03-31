// Fill out your copyright notice in the Description page of Project Settings.


#include "LootGenGameInstanceSubsystem.h"

#include "HInventoryItemInstance.h"
#include "HItemDefinition.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogHLootSubsystem);

void ULootGenGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOGFMT(LogHLootSubsystem, Warning, "Loot system initted");
}

void ULootGenGameInstanceSubsystem::Deinitialize()
{
	UE_LOGFMT(LogHLootSubsystem, Warning, "Loot system Deinitted");
}

UHInventoryItemInstance* ULootGenGameInstanceSubsystem::GenerateItemInstance(UHItemDefinition* ItemDef)
{
	if(!ItemDef)
	{
		return nullptr;
	}

	UHInventoryItemInstance* Instance = NewObject<UHInventoryItemInstance>(GetWorld());
	Instance->SetItemDef(ItemDef);
	for (UHInventoryItemFragment* Fragment : ItemDef->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(Instance);
		}
	}

	int randNum = FMath::RandRange(0, 5);

	UE_LOGFMT(LogHLootSubsystem, Warning, "Should have generated instance with num: {num}", randNum);

	return Instance;
}

void ULootGenGameInstanceSubsystem::GenerateItemInstanceFromSoftDel(TSoftObjectPtr<UHItemDefinition> ItemDefRef, const FHItemInstanceGenerated& Delegate)
{
	// Async load the indicator, and pool the results so that it's easy to use and reuse the widgets.
	if (!ItemDefRef.IsNull())
	{
		AsyncLoad(ItemDefRef, [this, ItemDefRef, Delegate]() {
			if (UHItemDefinition* ItemDef = ItemDefRef.Get())
			{
				UHInventoryItemInstance* GeneratedInstance = GenerateItemInstance(ItemDef);
				Delegate.ExecuteIfBound(GeneratedInstance);
			}
		});
		StartAsyncLoading();
	}
}
