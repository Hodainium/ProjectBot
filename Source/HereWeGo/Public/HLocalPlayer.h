// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "HLocalPlayer.generated.h"

class UHSettingsShared;
/**
 * CommonGame LocalPlayer
 *
 * Must be configured in DefaultEngine.ini:
 *
 * [/Script/Engine.Engine]
 * LocalPlayerClassName=/Script/HereWeGo.HLocalPlayer
 */
UCLASS()
class HEREWEGO_API UHLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

public:
	UHLocalPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Gets the shared setting for this player, this is read using the save game system so may not be correct until after user login */
	UFUNCTION()
	UHSettingsShared* GetSharedSettings() const;

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UHSettingsShared> SharedSettings;
};
