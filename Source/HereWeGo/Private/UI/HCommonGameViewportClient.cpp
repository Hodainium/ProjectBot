// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HCommonGameViewportClient.h"

#include "CommonUISettings.h"
#include "ICommonUIModule.h"

namespace GameViewportTags
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_HardwareCursor, "Platform.Trait.Input.HardwareCursor");
}


UHCommonGameViewportClient::UHCommonGameViewportClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHCommonGameViewportClient::Init(FWorldContext& WorldContext, UGameInstance* OwningGameInstance,
	bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);

	// We have software cursors set up in our project settings for console/mobile use, but on desktop
	// native hardware cursors are preferred.
	const bool bUseHardwareCursor = ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(GameViewportTags::TAG_Platform_Trait_Input_HardwareCursor);
	SetUseSoftwareCursorWidgets(!bUseHardwareCursor);
}
