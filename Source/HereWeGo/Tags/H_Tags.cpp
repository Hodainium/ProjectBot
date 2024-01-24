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

	UE_DEFINE_GAMEPLAY_TAG(TAG_ELIMINATION_MESSAGE, "ProjectBot.Elimination.Message");

}

namespace H_GameplayEvent_Tags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ABILITY_BEHAVIOR_SURVIVESDEATH, "Ability.Behavior.SurvivesDeath", "An ability with this type tag should not be canceled due to death.");

}