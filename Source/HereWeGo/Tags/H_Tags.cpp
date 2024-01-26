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
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GAMEPLAYEVENT_DEATH, "GameplayEvent.Death", "Event that fires on death. This event only fires on the server.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GAMEPLAYEVENT_RESET, "GameplayEvent.Reset", "Event that fires once a player reset is executed.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GAMEPLAYEVENT_REQUESTRESET, "GameplayEvent.RequestReset", "Event to request a player's pawn to be instantly replaced with a new one at a valid spawn location.");
}

namespace H_GameplayEvent_Tags
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
	UE_DEFINE_GAMEPLAY_TAG(TAG_ELIMINATION_MESSAGE, "ProjectBot.Elimination.Message");
	UE_DEFINE_GAMEPLAY_TAG(TAG_DAMAGE_MESSAGE, "ProjectBot.Damage.Message");

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