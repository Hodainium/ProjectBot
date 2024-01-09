// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Armor/HArmorPieceDefinition.h"
#include "UObject/NoExportTypes.h"
#include "HArmorInstance.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHArmorInstance : public UObject
{
	GENERATED_BODY()

public:
	UHArmorInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;
	///////

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<USkeletalMesh*> GetSpawnedMeshes() const { return SpawnedMeshes; }

	virtual void SpawnMeshes(const TArray<FHArmorMeshToSpawn>& MeshesToSpawn);
	virtual void DestroyMeshes();

	virtual void OnEquipped();
	virtual void OnUnequipped();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

private:
	UFUNCTION()
	void OnRep_Instigator();

private:
	UPROPERTY(ReplicatedUsing = OnRep_Instigator)
	TObjectPtr<UObject> Instigator;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<USkeletalMesh>> SpawnedMeshes;
};

