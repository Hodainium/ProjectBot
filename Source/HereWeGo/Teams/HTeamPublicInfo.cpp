// Fill out your copyright notice in the Description page of Project Settings.


#include "HTeamPublicInfo.h"

#include "HTeamInfoBase.h"
#include "Net/UnrealNetwork.h"

class FLifetimeProperty;

AHTeamPublicInfo::AHTeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AHTeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void AHTeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<UHTeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void AHTeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}



