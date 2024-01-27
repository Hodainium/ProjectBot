// Fill out your copyright notice in the Description page of Project Settings.


#include "HTeamAgentInterface.h"

#include "HLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HTeamAgentInterface)

UHTeamAgentInterface::UHTeamAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Add default functionality here for any IHTeamAgentInterface functions that are not pure virtual.
void IHTeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<IHTeamAgentInterface> This,
	FGenericTeamId OldTeamID, FGenericTeamId NewTeamID)
{
	if (OldTeamID != NewTeamID)
	{
		const int32 OldTeamIndex = GenericTeamIdToInteger(OldTeamID);
		const int32 NewTeamIndex = GenericTeamIdToInteger(NewTeamID);

		UObject* ThisObj = This.GetObject();
		UE_LOG(LogHGame, Verbose, TEXT("[%s] %s assigned team %d"), *GetClientServerContextString(ThisObj), *GetPathNameSafe(ThisObj), NewTeamIndex);

		This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
	}
}
