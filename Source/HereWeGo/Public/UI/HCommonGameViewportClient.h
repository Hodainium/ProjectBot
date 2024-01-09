// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "HCommonGameViewportClient.generated.h"

/**
 * CommonGame Game Viewport Client
 *
 * Must be configured in DefaultEngine.ini:
 *
 * [/Script/Engine.Engine]
 * GameViewportClientClassName=/Script/HereWeGo.HGameViewportClient
 */
UCLASS(BlueprintType)
class HEREWEGO_API UHCommonGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	UHCommonGameViewportClient(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UGameViewportClient interface
	virtual void Init(FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice) override;
	//~End of UGameViewportClient

};
