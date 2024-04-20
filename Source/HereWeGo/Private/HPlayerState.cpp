// Fill out your copyright notice in the Description page of Project Settings.

#include "HPlayerState.h"
#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"
#include "HEquipmentComponent.h"
#include "HInventoryComponent.h"
#include "HItemSlotComponent.h"
#include "HPlayerCharacter.h"
#include "HPlayerController.h"
#include "HPawnData.h"
#include "HWeaponComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

class AController;
class APlayerState;
class FLifetimeProperty;

const FName AHPlayerState::NAME_HAbilityReady("HAbilitiesReady");

AHPlayerState::AHPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MyPlayerConnectionType(EHPlayerConnectionType::Player)
{
	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;

	MyTeamID = FGenericTeamId::NoTeam;
	MySquadID = INDEX_NONE;
}

void AHPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AHPlayerState::Reset()
{
	Super::Reset();
}

void AHPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	/*if (UHPawnExtensionComponent* PawnExtComp = UHPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}*/
}

void AHPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void AHPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
	case EHPlayerConnectionType::Player:
	case EHPlayerConnectionType::InactivePlayer:
		//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
		// (e.g., for long running servers where they might build up if lots of players cycle through)
		bDestroyDeactivatedPlayerState = true;
		break;
	default:
		bDestroyDeactivatedPlayerState = true;
		break;
	}

	SetPlayerConnectionType(EHPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void AHPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == EHPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EHPlayerConnectionType::Player);
	}
}

//void AHPlayerState::OnExperienceLoaded(const UHExperienceDefinition* /*CurrentExperience*/)
//{
//	if (AHGameMode* HGameMode = GetWorld()->GetAuthGameMode<AHGameMode>())
//	{
//		if (const UHPawnData* NewPawnData = HGameMode->GetPawnDataForController(GetOwningController()))
//		{
//			SetPawnData(NewPawnData);
//		}
//		else
//		{
//			UE_LOG(LogH, Error, TEXT("AHPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
//		}
//	}
//}

void AHPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MySquadID, SharedParams);

	SharedParams.Condition = ELifetimeCondition::COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedViewRotation, SharedParams);

	DOREPLIFETIME(ThisClass, StatTags);
}

FRotator AHPlayerState::GetReplicatedViewRotation() const
{
	// Could replace this with custom replication
	return ReplicatedViewRotation;
}

void AHPlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)
{
	if (NewRotation != ReplicatedViewRotation)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedViewRotation, this);
		ReplicatedViewRotation = NewRotation;
	}
}

AHPlayerController* AHPlayerState::GetHPlayerController() const
{
	return Cast<AHPlayerController>(GetOwner());
}

void AHPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/*check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UHExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UHExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnHExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}*/
}

void AHPlayerState::SetPawnData(const UHPawnData* InPawnData)
{
	/*check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogHGame, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const UHAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_HAbilityReady);

	ForceNetUpdate();*/
}

void AHPlayerState::OnRep_PawnData()
{
}

void AHPlayerState::SetPlayerConnectionType(EHPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}

void AHPlayerState::SetSquadID(int32 NewSquadId)
{
	if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MySquadID, this);

		MySquadID = NewSquadId;
	}
}

void AHPlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		const FGenericTeamId OldTeamID = MyTeamID;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyTeamID, this);
		MyTeamID = NewTeamID;
		ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
	}
	else
	{
		UE_LOG(LogHTeams, Error, TEXT("Cannot set team for %s on non-authority"), *GetPathName(this));
	}
}

FGenericTeamId AHPlayerState::GetGenericTeamId() const
{
	return MyTeamID;
}

FHOnTeamIndexChangedDelegate* AHPlayerState::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void AHPlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AHPlayerState::OnRep_MySquadID()
{
	//@TODO: Let the squad subsystem know (once that exists)
}

void AHPlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void AHPlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 AHPlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool AHPlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void AHPlayerState::ClientBroadcastMessage_Implementation(const FHVerbMessage Message)
{
	// This check is needed to prevent running the action when in standalone mode
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

