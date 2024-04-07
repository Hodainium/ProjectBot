// Fill out your copyright notice in the Description page of Project Settings.


#include "HAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "HItemDefinition.h"
#include "HLogChannels.h"
#include "HGameData.h"
#include "Logging/StructuredLog.h"
#include "Misc/ScopedSlowTask.h"

const FPrimaryAssetType	UHAssetManager::UndefinedItemType = TEXT("UNDEFINED_Type");
const FPrimaryAssetType	UHAssetManager::ResourceItemType = TEXT("Resource_Type");
const FPrimaryAssetType	UHAssetManager::ArmorItemType = TEXT("Armor_Type");
const FPrimaryAssetType	UHAssetManager::WeaponItemType = TEXT("Weapon_Type");
const FPrimaryAssetType	UHAssetManager::ItemModItemType = TEXT("ItemMod_Type");
const FPrimaryAssetType	UHAssetManager::CharacterModItemType = TEXT("CharacterMod_Type");

UHAssetManager& UHAssetManager::Get()
{
	UHAssetManager* Singleton = Cast<UHAssetManager>(GEngine->AssetManager);

	if(Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be HAssetManager!"));
		return *NewObject<UHAssetManager>();
	}
}

const UHGameData& UHAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<UHGameData>(HGameDataPath);
}

void UHAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}

UHItemDefinition* UHAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UHItemDefinition* LoadedItem = Cast<UHItemDefinition>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogHGame, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}

void UHAssetManager::GetAllItemModsMatching(FHItemSearchQuery& SearchQuery, TArray<FAssetData>& OutAssets)
{
	TArray<FAssetData> ModAssetData;
	GetPrimaryAssetDataList(FPrimaryAssetType(ItemModItemType), ModAssetData);
	for (const auto& Asset : ModAssetData)
	{
		////Item quality
		//EHItemQuality OutQuality;
		//Asset.GetTagValue("Quality", OutQuality);

		//Asset tags
		FString OutAssetTagsString;
		Asset.GetTagValue("AssetTags", OutAssetTagsString);
		FGameplayTagContainer AssetTags;
		AssetTags.FromExportString(OutAssetTagsString);

		//Tags to grant during query
		FString OutTagsToGrantDuringQueryString;
		Asset.GetTagValue("TagsToGrantDuringQuery", OutTagsToGrantDuringQueryString);
		FGameplayTagContainer TagsToGrantDuringQuery;
		TagsToGrantDuringQuery.FromExportString(OutAssetTagsString);

		//Required tags
		FString OutRequiredTagsString;
		Asset.GetTagValue("RequiredTags", OutRequiredTagsString);
		FGameplayTagContainer RequiredTags;
		RequiredTags.FromExportString(OutRequiredTagsString);

		//Blocked tags
		FString OutBlockedTagsString;
		Asset.GetTagValue("BlockedTags", OutBlockedTagsString);
		FGameplayTagContainer BlockedTags;
		BlockedTags.FromExportString(OutBlockedTagsString);

		/*if (AllowedTags.HasTag(WeaponTag))
		{
			OutAssets.Add(Asset);
		}*/

		OutAssets.Add(Asset);
	}
}

UObject* UHAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool UHAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UHAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

UPrimaryDataAsset* UHAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass,
	const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("HEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(LogHGame, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(LogHGame, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}
