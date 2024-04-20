// Fill out your copyright notice in the Description page of Project Settings.


#include "HTeamStatics.h"

#include "HLogChannels.h"
#include "HTeamDisplayAsset.h"
#include "HTeamSubsystem.h"

void UHTeamStatics::FindTeamFromObject(const UObject* Agent, bool& bIsPartOfTeam, int32& TeamId, UHTeamDisplayAsset*& DisplayAsset, bool bLogIfNotSet)
{
	bIsPartOfTeam = false;
	TeamId = INDEX_NONE;
	DisplayAsset = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(Agent, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UHTeamSubsystem* TeamSubsystem = World->GetSubsystem<UHTeamSubsystem>())
		{
			TeamId = TeamSubsystem->FindTeamFromObject(Agent);
			if (TeamId != INDEX_NONE)
			{
				bIsPartOfTeam = true;

				DisplayAsset = TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);

				if ((DisplayAsset == nullptr) && bLogIfNotSet)
				{
					UE_LOG(LogHTeams, Log, TEXT("FindTeamFromObject(%s) called too early (found team %d but no display asset set yet"), *GetPathNameSafe(Agent), TeamId);
				}
			}
		}
		else
		{
			UE_LOG(LogHTeams, Error, TEXT("FindTeamFromObject(%s) failed: Team subsystem does not exist yet"), *GetPathNameSafe(Agent));
		}
	}
}

UHTeamDisplayAsset* UHTeamStatics::GetTeamDisplayAsset(const UObject* WorldContextObject, int32 TeamId)
{
	UHTeamDisplayAsset* Result = nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UHTeamSubsystem* TeamSubsystem = World->GetSubsystem<UHTeamSubsystem>())
		{
			return TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);
		}
	}
	return Result;
}

float UHTeamStatics::GetTeamScalarWithFallback(UHTeamDisplayAsset* DisplayAsset, FName ParameterName, float DefaultValue)
{
	if (DisplayAsset)
	{
		if (float* pValue = DisplayAsset->ScalarParameters.Find(ParameterName))
		{
			return *pValue;
		}
	}
	return DefaultValue;
}

FLinearColor UHTeamStatics::GetTeamColorWithFallback(UHTeamDisplayAsset* DisplayAsset, FName ParameterName, FLinearColor DefaultValue)
{
	if (DisplayAsset)
	{
		if (FLinearColor* pColor = DisplayAsset->ColorParameters.Find(ParameterName))
		{
			return *pColor;
		}
	}
	return DefaultValue;
}

UTexture* UHTeamStatics::GetTeamTextureWithFallback(UHTeamDisplayAsset* DisplayAsset, FName ParameterName, UTexture* DefaultValue)
{
	if (DisplayAsset)
	{
		if (TObjectPtr<UTexture>* pTexture = DisplayAsset->TextureParameters.Find(ParameterName))
		{
			return *pTexture;
		}
	}
	return DefaultValue;
}