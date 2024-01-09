// Fill out your copyright notice in the Description page of Project Settings.

#include "HHUDLayout.h"
#include "NativeGameplayTags.h"
#include "Input/CommonUIInputTypes.h"
#include "CommonUIExtensions.h"
#include "HereWeGo/Tags/H_Tags.h"


UHHUDLayout::UHHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(H_CommonUI_Tags::TAG_UI_ACTION_MAINMENU), false,
	                                          FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMainMenuAction)));
}

void UHHUDLayout::HandleMainMenuAction()
{
	if (ensure(!MainMenuClass.IsNull()))
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), H_CommonUI_Tags::TAG_UI_LAYER_MENU, MainMenuClass);
	}
}