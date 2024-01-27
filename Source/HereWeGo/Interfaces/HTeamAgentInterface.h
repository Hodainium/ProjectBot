// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "UObject/Interface.h"
#include "HTeamAgentInterface.generated.h"

template <typename InterfaceType> class TScriptInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHTeamIndexChangedDelegate, UObject*, ObjectChangingTeam, int32, OldTeamID, int32, NewTeamID);

inline int32 GenericTeamIdToInteger(FGenericTeamId ID)
{
	return (ID == FGenericTeamId::NoTeam) ? INDEX_NONE : (int32)ID;
}

inline FGenericTeamId IntegerToGenericTeamId(int32 ID)
{
	return (ID == INDEX_NONE) ? FGenericTeamId::NoTeam : FGenericTeamId((uint8)ID);
}

// This class does not need to be modified.
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UHTeamAgentInterface : public UGenericTeamAgentInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HEREWEGO_API IHTeamAgentInterface : public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	virtual FOnHTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() { return nullptr; }

	static void ConditionalBroadcastTeamChanged(TScriptInterface<IHTeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID);

	FOnHTeamIndexChangedDelegate& GetTeamChangedDelegateChecked()
	{
		FOnHTeamIndexChangedDelegate* Result = GetOnTeamIndexChangedDelegate();
		check(Result);
		return *Result;
	}
};
