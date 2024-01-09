// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/HAimSensitivityData.h"

#include "Saves/HSettingsShared.h"

UHAimSensitivityData::UHAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ EHGamepadSensitivity::Slow,			0.5f },
		{ EHGamepadSensitivity::SlowPlus,		0.75f },
		{ EHGamepadSensitivity::SlowPlusPlus,	0.9f },
		{ EHGamepadSensitivity::Normal,		1.0f },
		{ EHGamepadSensitivity::NormalPlus,	1.1f },
		{ EHGamepadSensitivity::NormalPlusPlus,1.25f },
		{ EHGamepadSensitivity::Fast,			1.5f },
		{ EHGamepadSensitivity::FastPlus,		1.75f },
		{ EHGamepadSensitivity::FastPlusPlus,	2.0f },
		{ EHGamepadSensitivity::Insane,		2.5f },
	};
}

const float UHAimSensitivityData::SensitivtyEnumToFloat(const EHGamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}