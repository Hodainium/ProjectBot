// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/HSettingsShared.h"

#include "Framework/Application/SlateApplication.h"
#include "Internationalization/Culture.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "Misc/ConfigCacheIni.h"
#include "HLocalPlayer.h"
#include "Rendering/SlateRenderer.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"

static FString SHARED_SETTINGS_SLOT_NAME = TEXT("SharedGameSettings");

namespace HSettingsSharedCVars
{
	static float DefaultGamepadLeftStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadLeftStickInnerDeadZone(
		TEXT("gpad.DefaultLeftStickInnerDeadZone"),
		DefaultGamepadLeftStickInnerDeadZone,
		TEXT("Gamepad left stick inner deadzone")
	);

	static float DefaultGamepadRightStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadRightStickInnerDeadZone(
		TEXT("gpad.DefaultRightStickInnerDeadZone"),
		DefaultGamepadRightStickInnerDeadZone,
		TEXT("Gamepad right stick inner deadzone")
	);
}

UHSettingsShared::UHSettingsShared()
{
	//FInternationalization::Get().OnCultureChanged().AddUObject(this, &UHSettingsShared::OnCultureChanged);

	GamepadMoveStickDeadZone = HSettingsSharedCVars::DefaultGamepadLeftStickInnerDeadZone;
	GamepadLookStickDeadZone = HSettingsSharedCVars::DefaultGamepadRightStickInnerDeadZone;
}

int32 UHSettingsShared::GetLatestDataVersion() const
{
	// 0 = before subclassing ULocalPlayerSaveGame
	// 1 = first proper version
	return 1;
}

UHSettingsShared* UHSettingsShared::CreateTemporarySettings(const UHLocalPlayer* LocalPlayer)
{
	// This is not loaded from disk but should be set up to save
	UHSettingsShared* SharedSettings = Cast<UHSettingsShared>(CreateNewSaveGameForLocalPlayer(UHSettingsShared::StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME));

	SharedSettings->ApplySettings();

	return SharedSettings;
}

UHSettingsShared* UHSettingsShared::LoadOrCreateSettings(const UHLocalPlayer* LocalPlayer)
{
	// This will stall the main thread while it loads
	UHSettingsShared* SharedSettings = Cast<UHSettingsShared>(LoadOrCreateSaveGameForLocalPlayer(UHSettingsShared::StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME));

	SharedSettings->ApplySettings();

	return SharedSettings;
}

bool UHSettingsShared::AsyncLoadOrCreateSettings(const UHLocalPlayer* LocalPlayer, FOnSettingsLoadedEvent Delegate)
{
	FOnLocalPlayerSaveGameLoadedNative Lambda = FOnLocalPlayerSaveGameLoadedNative::CreateLambda([Delegate]
	(ULocalPlayerSaveGame* LoadedSave)
	{
		UHSettingsShared* LoadedSettings = CastChecked<UHSettingsShared>(LoadedSave);

		LoadedSettings->ApplySettings();

		Delegate.ExecuteIfBound(LoadedSettings);
	});

	return ULocalPlayerSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(UHSettingsShared::StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME, Lambda);
}

void UHSettingsShared::SaveSettings()
{
	// Schedule an async save because it's okay if it fails
	AsyncSaveGameToSlotForLocalPlayer();

	// TODO_BH: Move this to the serialize function instead with a bumped version number
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->AsyncSaveSettings();
		}
	}
}

void UHSettingsShared::ApplySettings()
{
	//ApplySubtitleOptions();
	ApplyBackgroundAudioSettings();
	//ApplyCultureSettings();

	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->ApplySettings();
		}
	}
}

void UHSettingsShared::SetColorBlindStrength(int32 InColorBlindStrength)
{
	InColorBlindStrength = FMath::Clamp(InColorBlindStrength, 0, 10);
	if (ColorBlindStrength != InColorBlindStrength)
	{
		ColorBlindStrength = InColorBlindStrength;
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
			(EColorVisionDeficiency)(int32)ColorBlindMode, (int32)ColorBlindStrength, true, false);
	}
}

int32 UHSettingsShared::GetColorBlindStrength() const
{
	return ColorBlindStrength;
}

void UHSettingsShared::SetColorBlindMode(EColorBlindMode InMode)
{
	if (ColorBlindMode != InMode)
	{
		ColorBlindMode = InMode;
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
			(EColorVisionDeficiency)(int32)ColorBlindMode, (int32)ColorBlindStrength, true, false);
	}
}

EColorBlindMode UHSettingsShared::GetColorBlindMode() const
{
	return ColorBlindMode;
}

//void UHSettingsShared::ApplySubtitleOptions()
//{
//	/*if (USubtitleDisplaySubsystem* SubtitleSystem = USubtitleDisplaySubsystem::Get(OwningPlayer))
//	{
//		FSubtitleFormat SubtitleFormat;
//		SubtitleFormat.SubtitleTextSize = SubtitleTextSize;
//		SubtitleFormat.SubtitleTextColor = SubtitleTextColor;
//		SubtitleFormat.SubtitleTextBorder = SubtitleTextBorder;
//		SubtitleFormat.SubtitleBackgroundOpacity = SubtitleBackgroundOpacity;
//
//		SubtitleSystem->SetSubtitleDisplayOptions(SubtitleFormat);
//	}*/
//}

//////////////////////////////////////////////////////////////////////

void UHSettingsShared::SetAllowAudioInBackgroundSetting(EHAllowBackgroundAudioSetting NewValue)
{
	if (ChangeValueAndDirty(AllowAudioInBackground, NewValue))
	{
		ApplyBackgroundAudioSettings();
	}
}

void UHSettingsShared::ApplyBackgroundAudioSettings()
{
	if (OwningPlayer && OwningPlayer->IsPrimaryPlayer())
	{
		FApp::SetUnfocusedVolumeMultiplier((AllowAudioInBackground != EHAllowBackgroundAudioSetting::Off) ? 1.0f : 0.0f);
	}
}

//////////////////////////////////////////////////////////////////////

//void UHSettingsShared::ApplyCultureSettings()
//{
//	if (bResetToDefaultCulture)
//	{
//		const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
//		check(SystemDefaultCulture.IsValid());
//
//		const FString CultureToApply = SystemDefaultCulture->GetName();
//		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
//		{
//			// Clear this string
//			GConfig->RemoveKey(TEXT("Internationalization"), TEXT("Culture"), GGameUserSettingsIni);
//			GConfig->Flush(false, GGameUserSettingsIni);
//		}
//		bResetToDefaultCulture = false;
//	}
//	else if (!PendingCulture.IsEmpty())
//	{
//		// SetCurrentCulture may trigger PendingCulture to be cleared (if a culture change is broadcast) so we take a copy of it to work with
//		const FString CultureToApply = PendingCulture;
//		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
//		{
//			// Note: This is intentionally saved to the users config
//			// We need to localize text before the player logs in and very early in the loading screen
//			GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *CultureToApply, GGameUserSettingsIni);
//			GConfig->Flush(false, GGameUserSettingsIni);
//		}
//		ClearPendingCulture();
//	}
//}
//
//void UHSettingsShared::ResetCultureToCurrentSettings()
//{
//	ClearPendingCulture();
//	bResetToDefaultCulture = false;
//}
//
//const FString& UHSettingsShared::GetPendingCulture() const
//{
//	return PendingCulture;
//}
//
//void UHSettingsShared::SetPendingCulture(const FString& NewCulture)
//{
//	PendingCulture = NewCulture;
//	bResetToDefaultCulture = false;
//	bIsDirty = true;
//}
//
//void UHSettingsShared::OnCultureChanged()
//{
//	ClearPendingCulture();
//	bResetToDefaultCulture = false;
//}
//
//void UHSettingsShared::ClearPendingCulture()
//{
//	PendingCulture.Reset();
//}
//
//bool UHSettingsShared::IsUsingDefaultCulture() const
//{
//	FString Culture;
//	GConfig->GetString(TEXT("Internationalization"), TEXT("Culture"), Culture, GGameUserSettingsIni);
//
//	return Culture.IsEmpty();
//}
//
//void UHSettingsShared::ResetToDefaultCulture()
//{
//	ClearPendingCulture();
//	bResetToDefaultCulture = true;
//	bIsDirty = true;
//}

//////////////////////////////////////////////////////////////////////

void UHSettingsShared::ApplyInputSensitivity()
{

}