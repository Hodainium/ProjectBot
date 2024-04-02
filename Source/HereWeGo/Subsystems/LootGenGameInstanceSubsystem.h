// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncMixin.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LootGenGameInstanceSubsystem.generated.h"

enum class EHLootQuality : uint8;
class UHItemDefinition;
class UHInventoryItemInstance;

DECLARE_LOG_CATEGORY_EXTERN(LogHLootSubsystem, Log, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FHItemInstanceGenerated, UHInventoryItemInstance*, GeneratedInstance);

/**
 * 
 */
UCLASS()
class HEREWEGO_API ULootGenGameInstanceSubsystem : public UGameInstanceSubsystem, public FAsyncMixin
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/*UFUNCTION(BlueprintCallable, Category = "LootGenSystem")
	UHInventoryItemInstance* GenerateItemInstance();

	UFUNCTION(BlueprintCallable, Category = "LootGenSystem")
	UHInventoryItemInstance* GenerateItemInstanceFromPool(UHItemPool* Pool);*/

	UFUNCTION(BlueprintCallable, Category = "LootGenSystem")
	UHInventoryItemInstance* GenerateItemInstance(UHItemDefinition* ItemDef);

	UFUNCTION(BlueprintCallable, Category = "LootGenSystem", meta = (AutoCreateRefTerm = "Delegate"))
	void GenerateItemInstanceFromSoftDel(TSoftObjectPtr<UHItemDefinition> ItemDefRef, const FHItemInstanceGenerated& Delegate);

protected:

	EHLootQuality GenerateLootQuality();
};
