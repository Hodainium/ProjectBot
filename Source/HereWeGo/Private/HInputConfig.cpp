// Fill out your copyright notice in the Description page of Project Settings.


#include "HInputConfig.h"
#include "GameplayTagContainer.h"
#include "HLogChannels.h"
#include "InputAction.h"
#include "UObject/NameTypes.h"
#include "InputAction.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Movement, "InputAction.Player.Movement");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_LookMouse, "InputAction.Player.Look.Mouse");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_LookStick, "InputAction.Player.Look.Stick");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Run, "InputAction.Player.Run");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_AutoRun, "InputAction.Player.AutoRun");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Jump, "InputAction.Player.Jump");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Crouch, "InputAction.Player.Crouch");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Equipment_1, "InputAction.Player.Equipment.Slot1");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Equipment_2, "InputAction.Player.Equipment.Slot2");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Equipment_3, "InputAction.Player.Equipment.Slot3");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Equipment_4, "InputAction.Player.Equipment.Slot4");

UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Ability_Attack_1, "InputAction.Player.Ability.Attack1");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Ability_Attack_2, "InputAction.Player.Ability.Attack2");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Ability_Interact, "InputAction.Player.Ability.Interact");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Ability_1, "InputAction.Player.Ability.Slot1");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Ability_2, "InputAction.Player.Ability.Slot2");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Ability_3, "InputAction.Player.Ability.Slot3");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Ability_4, "InputAction.Player.Ability.Slot4");

UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Inventory, "InputAction.Menu.Ability.Inventory");
UE_DEFINE_GAMEPLAY_TAG(TAG_InputAction_Pause, "InputAction.Menu.Ability.Pause");


UHInputConfig::UHInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UHInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FHInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogHGame, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UHInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FHInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogHGame, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
