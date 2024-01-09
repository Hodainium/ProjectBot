// Fill out your copyright notice in the Description page of Project Settings.


#include "HLocalPlayer.h"

#include "Saves/HSettingsShared.h"

UHLocalPlayer::UHLocalPlayer(const FObjectInitializer& ObjectInitializer) // UCommonLocalPlayer v5.3 doesn't support ObjectInitializer :/
{
}

UHSettingsShared* UHLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		// On PC it's okay to use the sync load because it only checks the disk
		// This could use a platform tag to check for proper save support instead
		bool bCanLoadBeforeLogin = PLATFORM_DESKTOP;

		if (bCanLoadBeforeLogin)
		{
			SharedSettings = UHSettingsShared::LoadOrCreateSettings(this);
		}
		else
		{
			// We need to wait for user login to get the real settings so return temp ones
			SharedSettings = UHSettingsShared::CreateTemporarySettings(this);
		}
	}

	return SharedSettings;
}
