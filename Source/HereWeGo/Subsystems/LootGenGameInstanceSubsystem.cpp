// Fill out your copyright notice in the Description page of Project Settings.


#include "LootGenGameInstanceSubsystem.h"

#include "HInventoryItemInstance.h"
#include "HItemDefinition.h"
#include "HereWeGo/HAssetManager.h"
#include "HereWeGo/DeveloperSettings/HLootSettings.h"
#include "HereWeGo/Items/LootGen/HItemAssetFilter.h"
#include "HereWeGo/Items/Modifiers/HItemModDefinition.h"
#include "HereWeGo/Items/Modifiers/HItemModInstance.h"
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

	return Instance;
}

void ULootGenGameInstanceSubsystem::GetCompatibleModAssetsForItemInstance(UHInventoryItemInstance* ItemInstance,
                                                                          TArray<FAssetData>& OutDataArray)
{
	FHItemSearchQuery Query = FHItemSearchQuery();
	//Query.BlockedModQualities = GetBlockedItemQualitiesForRange(EHItemQuality::Quality0, ItemInstance->GetItemQuality());

	UHAssetManager::Get().GetAllItemModsMatching(Query, OutDataArray);
}

void ULootGenGameInstanceSubsystem::GenerateItemInstanceFromSoftDel(TSoftObjectPtr<UHItemDefinition> ItemDefRef, const FHItemInstanceGenerated& Delegate)
{
	// Async load the indicator, and pool the results so that it's easy to use and reuse the widgets.
	if (!ItemDefRef.IsNull())
	{
		AsyncLoad(ItemDefRef, [this, ItemDefRef, Delegate]() 
		{
			if (UHItemDefinition* ItemDef = ItemDefRef.Get())
			{
				UHInventoryItemInstance* WeaponInstance = GenerateItemInstance(ItemDef);

				TArray<FAssetData> TotalModData;
				GetCompatibleModAssetsForItemInstance(WeaponInstance, TotalModData);

				TArray<FPrimaryAssetId> SelectedModIDs;
				TArray<EHItemQuality> SelectedQualities;

				int numMods = GenerateNumMods(WeaponInstance->GetItemQuality()) + 1; //TODO: Make a cvar

				UE_LOGFMT(LogHLootSubsystem, Warning, "Num mods: {0}", numMods);

				for (int i = 0; i < numMods; i++)
				{
					bool bModFound = false;

					EHItemQuality RolledModQuality = GenerateItemQuality(); //GenRarity()

					int j = 0;

					do
					{ 
						int maxIndex = TotalModData.Num() - 1 - j;
						int randIndex = FMath::RandRange(0, maxIndex);

						////Item quality
						//TSet<EHItemQuality> OutQualities;
						//TotalModData[randIndex].GetTagValue("AvailableQualities", OutQualities);
						//TODO Tset doesnt work will need to do bitmap

						//if(OutQualities.Contains(RolledModQuality))
						//{
						//	
						//}

						TotalModData.Swap(randIndex, TotalModData.Num() - 1 - j);
						SelectedModIDs.Add(TotalModData[randIndex].GetPrimaryAssetId());
						SelectedQualities.Add(RolledModQuality);
						bModFound = true;

						UE_LOGFMT(LogHLootSubsystem, Warning, "Mod found!!!");

					} while (!bModFound && j < TotalModData.Num());
				}

				TArray<FName> Bundles;

				AsyncPreloadPrimaryAssetsAndBundles(SelectedModIDs, Bundles, [this, SelectedModIDs, SelectedQualities, WeaponInstance, Delegate]()
				{
					for (int i = 0; i < SelectedModIDs.Num(); i++)
					{
						if (UHItemModDefinition* ModDef = Cast<UHItemModDefinition>(UHAssetManager::Get().GetPrimaryAssetObject(SelectedModIDs[i])))
						{
							UHItemModInstance* ModInstance = NewObject<UHItemModInstance>(WeaponInstance);  //@TODO: Using the actor instead of component as the outer due to UE-127172
							ModInstance->SetModDefinition(ModDef);
							ModInstance->SetModQuality(SelectedQualities[i]);

							//ModInstance->SetModLevelOffset();
							WeaponInstance->AddItemMod(ModInstance);
						}
					}
					Delegate.ExecuteIfBound(WeaponInstance);
				});
				StartAsyncLoading();
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
