// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HGameplayTagStackContainer.h"
#include "HTeamInfoBase.h"
#include "HTeamPublicInfo.generated.h"

class UHTeamCreationComponent;
class UHTeamDisplayAsset;
class UObject;
struct FFrame;

UCLASS()
class AHTeamPublicInfo : public AHTeamInfoBase
{
	GENERATED_BODY()

	friend UHTeamCreationComponent;

public:
	AHTeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UHTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<UHTeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamDisplayAsset)
	TObjectPtr<UHTeamDisplayAsset> TeamDisplayAsset;
};
