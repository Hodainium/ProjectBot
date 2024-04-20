// Fill out your copyright notice in the Description page of Project Settings.


#include "HTeamInfoBase.h"

#include "HTeamSubsystem.h"
#include "Net/UnrealNetwork.h"

class FLifetimeProperty;

AHTeamInfoBase::AHTeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamId(INDEX_NONE)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false);
}

void AHTeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamId, COND_InitialOnly);
}

void AHTeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void AHTeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		UHTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UHTeamSubsystem>();
		if (TeamSubsystem)
		{
			// EndPlay can happen at weird times where the subsystem has already been destroyed
			TeamSubsystem->UnregisterTeamInfo(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AHTeamInfoBase::RegisterWithTeamSubsystem(UHTeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void AHTeamInfoBase::TryRegisterWithTeamSubsystem()
{
	if (TeamId != INDEX_NONE)
	{
		UHTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UHTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			RegisterWithTeamSubsystem(TeamSubsystem);
		}
	}
}

void AHTeamInfoBase::SetTeamId(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;

	TryRegisterWithTeamSubsystem();
}

void AHTeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamSubsystem();
}


