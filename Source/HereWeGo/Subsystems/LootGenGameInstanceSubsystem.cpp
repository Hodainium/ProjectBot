// Fill out your copyright notice in the Description page of Project Settings.


#include "LootGenGameInstanceSubsystem.h"

#include "HInventoryItemInstance.h"
#include "HItemDefinition.h"
#include "HereWeGo/HAssetManager.h"
#include "HereWeGo/DeveloperSettings/HLootSettings.h"
#include "HereWeGo/Items/LootGen/HItemAssetFilter.h"
#include "HereWeGo/Items/Modifiers/HItemModDefinition.h"
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

int ULootGenGameInstanceSubsystem::GenerateNumMods(EHItemQuality InQuality)
{
	return FMath::RandRange(static_cast<int>(InQuality) / 2, static_cast<int>(InQuality) + 1);
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

	EHItemQuality ItemQuality = GenerateItemQuality();
	Instance->SetItemQuality(ItemQuality);

	FName RowKey = GetRandomAdjectiveRowKey();

	if(RowKey.IsValid())
	{
		Instance->SetItemAdjectiveText(RowKey);
	}

	GenerateModsForItemInstance(Instance);

	return Instance;
}

void ULootGenGameInstanceSubsystem::GenerateModsForItemInstance(UHInventoryItemInstance* ItemInstance)
{
	//Generate mods here

	int numMods = GenerateNumMods(ItemInstance->GetItemQuality());
	UE_LOGFMT(LogHLootSubsystem, Warning, "Num mods produced: {num}", numMods);

	FHItemSearchQuery Query = FHItemSearchQuery();
	TArray<FAssetData> ModAssetData;

	UHAssetManager::Get().GetAllItemModsMatching(Query, ModAssetData);

	TArray<FAssetData> SelectedMods;

	for (int i = 0; i < numMods; i++) //const auto& Data : ModAssetData
	{
		UE_LOGFMT(LogHLootSubsystem, Warning, "Found mod: {mod}", ModAssetData[i].AssetName);

		//EHItemQuality ModQuality = GenerateItemQuality();

		//Pick random indexes here

		int randIndex = FMath::RandRange(0, ModAssetData.Num()-1);

		SelectedMods.Add(ModAssetData[randIndex]);
		ModAssetData.RemoveAt(randIndex);
	}

	/*for (const auto& Data : SelectedMods)
	{
		UE_LOGFMT(LogHLootSubsystem, Warning, "Found mod: {mod}", Data.AssetName);

		FPrimaryAssetId AssetID = Data.GetPrimaryAssetId();

		if (AssetID.IsValid())
		{
			AsyncLoad(AssetID, [this, AssetID]() {
				if (UHItemModDefinition* ModDef = UHAssetManager::GetPrimaryAssetObject(AssetID))
				{

				}
			});
			StartAsyncLoading();
		}
	}*/

	EHItemQuality ModQuality = GenerateItemQuality();

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

EHItemQuality ULootGenGameInstanceSubsystem::GenerateItemQuality()
{
	//Can this be an int?
	float randNum = FMath::RandRange(0.f, 100.f);

	EHItemQuality ItemQuality = EHItemQuality::Quality0;

	//Maybe move to function. GetItemQualityForVal(randNum)?
	//This will need to be more extendable to allow for items like the isaac sacred orb that enhances item rolls

	if (randNum < 1.f)
	{
		ItemQuality = EHItemQuality::Quality4;
	}
	else if (randNum < 7.77f)
	{
		ItemQuality = EHItemQuality::Quality3;
	}
	else if (randNum < 20.f)
	{
		ItemQuality = EHItemQuality::Quality2;
	}
	else if (randNum < 50.f)
	{
		ItemQuality = EHItemQuality::Quality1;
	}
	else
	{
		ItemQuality = EHItemQuality::Quality0;
	}

	return ItemQuality;
}

TSet<EHItemQuality> ULootGenGameInstanceSubsystem::GetBlockedItemQualitiesForRange(EHItemQuality inMin, EHItemQuality inMax)
{
	TSet<EHItemQuality> OutSet;

	for (EHItemQuality Quality : TEnumRange<EHItemQuality>())
	{
		if (!(Quality >= inMin && Quality <= inMax))
		{
			OutSet.Add(Quality);
		}
	}

	return OutSet;
}
