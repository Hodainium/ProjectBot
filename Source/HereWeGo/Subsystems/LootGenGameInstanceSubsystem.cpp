// Fill out your copyright notice in the Description page of Project Settings.


#include "LootGenGameInstanceSubsystem.h"

#include "HInventoryItemInstance.h"
#include "HItemDefinition.h"
#include "HereWeGo/HAssetManager.h"
#include "HereWeGo/DeveloperSettings/HLootSettings.h"
#include "HereWeGo/Items/LootGen/HItemAssetFilter.h"
#include "HereWeGo/Items/Modifiers/HItemModDefinition.h"
#include "HereWeGo/Items/Modifiers/HItemModInstance.h"
#include "HereWeGo/Tags/H_Tags.h"
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
	return FMath::RandRange((static_cast<int>(InQuality) + 1) / 2, static_cast<int>(InQuality) + 1);
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

				FHItemSearchQuery Query = FHItemSearchQuery();
				//Query.BlockedModQualities = GetBlockedItemQualitiesForRange(EHItemQuality::Quality0, ItemInstance->GetItemQuality());
				UHAssetManager::Get().GetAllItemModsMatching(Query, TotalModData);

				TArray<FPrimaryAssetId> SelectedModIDs;
				TArray<EHItemQuality> SelectedQualities;

				int numMods = GenerateNumMods(WeaponInstance->GetItemQuality()); // +1 TODO: Make this a cvar

				UE_LOGFMT(LogHLootSubsystem, Warning, "Num mods: {0}", numMods);

				
				
				//Find mods to add
				for (int i = 0; i < numMods; i++)
				{
					bool bModFound = false;

					//static_cast<EHItemQuality>((static_cast<int>(WeaponInstance->GetItemQuality()) + 1))

					EHItemQuality MinQuality = static_cast<EHItemQuality>(static_cast<int>(WeaponInstance->GetItemQuality()) / 2);

					EHItemQuality RolledModQuality = GenerateItemQuality(MinQuality, WeaponInstance->GetItemQuality()); //GenRarity()

					int offsetFromEndIdx = 0;

					do
					{ 
						int maxIndex = TotalModData.Num() - 1 - offsetFromEndIdx;
						int currentIndex = FMath::RandRange(0, maxIndex);

						//Item quality
						FString OutQualitiesString;
						TotalModData[currentIndex].GetTagValue(GET_MEMBER_NAME_CHECKED(UHItemModDefinition, AvailableQualities), OutQualitiesString);
						FGameplayTagContainer QualityTags;
						QualityTags.FromExportString(OutQualitiesString);

						if(QualityTags.HasTag(ConvertQualityEnumToTag(RolledModQuality)))
						{
							SelectedModIDs.Add(TotalModData[currentIndex].GetPrimaryAssetId());
							SelectedQualities.Add(RolledModQuality);
							TotalModData.RemoveAt(currentIndex);
							bModFound = true;

							UE_LOGFMT(LogHLootSubsystem, Warning, "Mod found!!!");
						}
						else
						{
							TotalModData.Swap(currentIndex, maxIndex);

							offsetFromEndIdx++;
						}
					} while (!bModFound && offsetFromEndIdx < TotalModData.Num());
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

EHItemQuality ULootGenGameInstanceSubsystem::GenerateItemQuality(EHItemQuality MinQuality, EHItemQuality MaxQuality)
{
	//Can this be an int?
	float randNum = FMath::RandRange(0.f, 100.f);

	EHItemQuality ItemQuality;

	//Maybe move to function. GetItemQualityForVal(randNum)?
	//This will need to be more extendable to allow for items like the isaac sacred orb that enhances item rolls
	//TODO add addition here for each weight. EX: randum < 1f + q4extraWeight

	if (randNum < 1.f) //+ extra q4 weight
	{
		ItemQuality = EHItemQuality::Quality4;
	}
	else if (randNum < 7.77f) //+ extra q4 weight + extra q3 weight?
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

	if (ItemQuality < MinQuality)
	{
		ItemQuality = MinQuality;
	}

	if(ItemQuality > MaxQuality)
	{
		ItemQuality = MaxQuality;
	}

	return ItemQuality;
}

FGameplayTag ULootGenGameInstanceSubsystem::ConvertQualityEnumToTag(EHItemQuality InQuality)
{
	switch(InQuality)
	{
	case EHItemQuality::Quality0:
		return H_ItemQuality_Tags::TAG_ITEM_QUALITY_0;
	case EHItemQuality::Quality1:
		return H_ItemQuality_Tags::TAG_ITEM_QUALITY_1;
	case EHItemQuality::Quality2:
		return H_ItemQuality_Tags::TAG_ITEM_QUALITY_2;
	case EHItemQuality::Quality3:
		return H_ItemQuality_Tags::TAG_ITEM_QUALITY_3;
	case EHItemQuality::Quality4:
		return H_ItemQuality_Tags::TAG_ITEM_QUALITY_4;
	case EHItemQuality::Quality5:
		return H_ItemQuality_Tags::TAG_ITEM_QUALITY_5;
	default:
		{
			UE_LOGFMT(LogHLootSubsystem, Error, "Invalid or not implemented loot enum passed to generate tag. Giving Q0");

			return H_ItemQuality_Tags::TAG_ITEM_QUALITY_0;
		}
	}
}
