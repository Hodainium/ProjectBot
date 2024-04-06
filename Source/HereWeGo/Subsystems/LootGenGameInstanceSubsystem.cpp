// Fill out your copyright notice in the Description page of Project Settings.


#include "LootGenGameInstanceSubsystem.h"

#include "HInventoryItemInstance.h"
#include "HItemDefinition.h"
#include "HereWeGo/DeveloperSettings/HLootSettings.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogHLootSubsystem);

void ULootGenGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UHLootSettings* LootSettings = GetDefault<UHLootSettings>(); // Access via CDO
	// Access defaults from DefaultGame.ini
	CachedAdjectiveTable = LootSettings->AdjectiveTable.LoadSynchronous();

	UE_LOGFMT(LogHLootSubsystem, Warning, "Loot system initted");
}

void ULootGenGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOGFMT(LogHLootSubsystem, Warning, "Loot system Deinitted");
}

FText ULootGenGameInstanceSubsystem::GetAdjectiveForKey(FName InKey)
{
	if (CachedAdjectiveTable)
	{
		if(FHTextRow* Row = CachedAdjectiveTable->FindRow<FHTextRow>(InKey, ""))
		{
			return Row->Text;
		}
	}

	return FText::GetEmpty();
}

FName ULootGenGameInstanceSubsystem::GetRandomAdjectiveRowKey()
{
	if (CachedAdjectiveTable)
	{
		TArray<FName> AdjectiveRowNames = CachedAdjectiveTable->GetRowNames();

		if (AdjectiveRowNames.Num() > 0)
		{
			int randIndex = FMath::RandRange(0, AdjectiveRowNames.Num() - 1);

			return AdjectiveRowNames[randIndex];
		}
	}

	return NAME_None;
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

	EHLootQuality LootQuality = GenerateLootQuality();
	Instance->SetItemQuality(LootQuality);

	FName RowKey = GetRandomAdjectiveRowKey();

	if(RowKey.IsValid())
	{
		Instance->SetItemAdjectiveText(RowKey);
	}

	UE_LOGFMT(LogHLootSubsystem, Warning, "Should have generated instance with num: {num}", UEnum::GetValueAsString(LootQuality));

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

EHLootQuality ULootGenGameInstanceSubsystem::GenerateLootQuality()
{
	//Can this be an int?
	float randNum = FMath::RandRange(0.f, 100.f);

	EHLootQuality ItemQuality = EHLootQuality::Quality0;

	//Maybe move to function. GetItemQualityForVal(randNum)?
	//This will need to be more extendable to allow for items like the isaac sacred orb that enhances item rolls

	if (randNum < 1.f)
	{
		ItemQuality = EHLootQuality::Quality4;
	}
	else if (randNum < 7.77f)
	{
		ItemQuality = EHLootQuality::Quality3;
	}
	else if (randNum < 20.f)
	{
		ItemQuality = EHLootQuality::Quality2;
	}
	else if (randNum < 50.f)
	{
		ItemQuality = EHLootQuality::Quality1;
	}
	else
	{
		ItemQuality = EHLootQuality::Quality0;
	}

	return ItemQuality;
}
