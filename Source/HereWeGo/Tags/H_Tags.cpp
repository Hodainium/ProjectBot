// Fill out your copyright notice in the Description page of Project Settings.


#include "H_Tags.h"



namespace H_CommonUI_Tags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_UI_ACTION_MAINMENU, "UI.Action.MainMenu");

	UE_DEFINE_GAMEPLAY_TAG(TAG_UI_LAYER_GAME, "UI.Layer.Game");
	UE_DEFINE_GAMEPLAY_TAG(TAG_UI_LAYER_GAMEMENU, "UI.Layer.GameMenu");
	UE_DEFINE_GAMEPLAY_TAG(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
	UE_DEFINE_GAMEPLAY_TAG(TAG_UI_LAYER_MODAL, "UI.Layer.Modal");
}

namespace H_GameplayEvent_Tags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GAMEPLAYEVENT_SHIELDBREAK, "GameplayEvent.ShieldBreak", "Event that fires on shieldbreak. This event only fires on the server.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GAMEPLAYEVENT_DEATH, "GameplayEvent.Death", "Event that fires on death. This event only fires on the server.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GAMEPLAYEVENT_RESET, "GameplayEvent.Reset", "Event that fires once a player reset is executed.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GAMEPLAYEVENT_REQUESTRESET, "GameplayEvent.RequestReset", "Event to request a player's pawn to be instantly replaced with a new one at a valid spawn location.");
}

namespace H_Ability_Tags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ABILITY_BEHAVIOR_SURVIVESDEATH, "Ability.Behavior.SurvivesDeath", "An ability with this type tag should not be canceled due to death.");
}

namespace H_Status_Tags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STATUS_CROUCHING, "Status.Crouching", "Target is crouching.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STATUS_AUTORUNNING, "Status.AutoRunning", "Target is auto-running.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STATUS_DEATH, "Status.Death", "Target has the death status.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STATUS_DEATH_DYING, "Status.Death.Dying", "Target has begun the death process.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STATUS_DEATH_DEAD, "Status.Death.Dead", "Target has finished the death process.");
}

namespace H_Message_Tags
{

	UE_DEFINE_GAMEPLAY_TAG(TAG_SHIELDBREAK_MESSAGE, "ProjectBot.Message.Shieldbreak");
	UE_DEFINE_GAMEPLAY_TAG(TAG_ELIMINATION_MESSAGE, "ProjectBot.Message.Elimination");
	UE_DEFINE_GAMEPLAY_TAG(TAG_DAMAGE_MESSAGE, "ProjectBot.Message.Damage");

}

namespace H_SetByCaller_Tags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_SETBYCALLER_DAMAGE, "SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_SETBYCALLER_HEAL, "SetByCaller.Heal", "SetByCaller tag used by healing gameplay effects.");
}

namespace H_Damage_Tags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_DAMAGE_IMMUNITY, "Damage.SelfDestruct");
	UE_DEFINE_GAMEPLAY_TAG(TAG_DAMAGE_SELFDESTRUCT, "Damage.FellOutOfWorld");
	UE_DEFINE_GAMEPLAY_TAG(TAG_DAMAGE_FELLOUTOFWORLD, "Damage.FellOutOfWorld");
}

namespace H_Cheat_Tags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_CHEAT_GODMODE, "Cheat.GodMode", "GodMode cheat is active on the owner.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_CHEAT_UNLIMITEDHEALTH, "Cheat.UnlimitedHealth", "UnlimitedHealth cheat is active on the owner.");
}

namespace H_MovementMode_Tags
{
	// These are mapped to the movement modes inside GetMovementModeTagMap()
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_MOVEMENT_MODE_WALKING, "Movement.Mode.Walking", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_MOVEMENT_MODE_NAVWALKING, "Movement.Mode.NavWalking", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_MOVEMENT_MODE_FALLING, "Movement.Mode.Falling", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_MOVEMENT_MODE_SWIMMING, "Movement.Mode.Swimming", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_MOVEMENT_MODE_FLYING, "Movement.Mode.Flying", "Default Character movement tag");

	// When extending, you can create your own movement modes but you need to update GetCustomMovementModeTagMap()
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_MOVEMENT_MODE_CUSTOM, "Movement.Mode.Custom", "This is invalid and should be replaced with custom tags.  See HGameplayTags::CustomMovementModeTagMap.");

	// Unreal Movement Modes
	const TMap<uint8, FGameplayTag> MovementModeTagMap =
	{
		{ MOVE_Walking, TAG_MOVEMENT_MODE_WALKING },
		{ MOVE_NavWalking, TAG_MOVEMENT_MODE_NAVWALKING },
		{ MOVE_Falling, TAG_MOVEMENT_MODE_FALLING },
		{ MOVE_Swimming, TAG_MOVEMENT_MODE_SWIMMING },
		{ MOVE_Flying, TAG_MOVEMENT_MODE_FLYING },
		{ MOVE_Custom, TAG_MOVEMENT_MODE_CUSTOM }
	};

	// Custom Movement Modes
	const TMap<uint8, FGameplayTag> CustomMovementModeTagMap =
	{
		// Fill these in with your custom modes
	};
}

namespace H_Movement_Tags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_GAMEPLAY_MOVEMENTSTOPPED, "Movement.Modifier.MovementStopped");
}

namespace H_ItemSlots_Tags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEMSLOTS_MESSAGE_SLOTSCHANGED, "Inventory.ItemSlots.Message.SlotsChanged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEMSLOTS_MESSAGE_ACTIVEINDEXCHANGED, "Inventory.ItemSlots.Message.ActiveIndexChanged");
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEMSLOTS_MESSAGE_NUMSLOTSCHANGED, "Inventory.ItemSlots.Message.NumSlotsChanged");
}

namespace H_ItemQuality_Tags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEM_QUALITY_0, "Item.Quality.0");
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEM_QUALITY_1, "Item.Quality.1");
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEM_QUALITY_2, "Item.Quality.2");
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEM_QUALITY_3, "Item.Quality.3");
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEM_QUALITY_4, "Item.Quality.4");
	UE_DEFINE_GAMEPLAY_TAG(TAG_ITEM_QUALITY_5, "Item.Quality.5");
}
