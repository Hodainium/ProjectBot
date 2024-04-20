// Fill out your copyright notice in the Description page of Project Settings.


#include "HTeamSubsystem.h"

#include "AbilitySystemGlobals.h"
#include "HLogChannels.h"
#include "HPlayerState.h"
#include "HTeamInfoBase.h"
#include "HTeamPrivateInfo.h"
#include "HTeamPublicInfo.h"
#include "HereWeGo/Interfaces/HTeamAgentInterface.h"

class FSubsystemCollectionBase;

//////////////////////////////////////////////////////////////////////
// FHTeamTrackingInfo

void FHTeamTrackingInfo::SetTeamInfo(AHTeamInfoBase* Info)
{
	if (AHTeamPublicInfo* NewPublicInfo = Cast<AHTeamPublicInfo>(Info))
	{
		ensure((PublicInfo == nullptr) || (PublicInfo == NewPublicInfo));
		PublicInfo = NewPublicInfo;

		UHTeamDisplayAsset* OldDisplayAsset = DisplayAsset;
		DisplayAsset = NewPublicInfo->GetTeamDisplayAsset();

		if (OldDisplayAsset != DisplayAsset)
		{
			OnTeamDisplayAssetChanged.Broadcast(DisplayAsset);
		}
	}
	else if (AHTeamPrivateInfo* NewPrivateInfo = Cast<AHTeamPrivateInfo>(Info))
	{
		ensure((PrivateInfo == nullptr) || (PrivateInfo == NewPrivateInfo));
		PrivateInfo = NewPrivateInfo;
	}
	else
	{
		checkf(false, TEXT("Expected a public or private team info but got %s"), *GetPathNameSafe(Info))
	}
}

void FHTeamTrackingInfo::RemoveTeamInfo(AHTeamInfoBase* Info)
{
	if (PublicInfo == Info)
	{
		PublicInfo = nullptr;
	}
	else if (PrivateInfo == Info)
	{
		PrivateInfo = nullptr;
	}
	else
	{
		ensureMsgf(false, TEXT("Expected a previously registered team info but got %s"), *GetPathNameSafe(Info));
	}
}

//////////////////////////////////////////////////////////////////////
// UHTeamSubsystem

UHTeamSubsystem::UHTeamSubsystem()
{
}

void UHTeamSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	/*auto AddTeamCheats = [](UCheatManager* CheatManager)
	{
		CheatManager->AddCheatManagerExtension(NewObject<UHTeamCheats>(CheatManager));
	};

	CheatManagerRegistrationHandle = UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(AddTeamCheats));*/
}

void UHTeamSubsystem::Deinitialize()
{
	//UCheatManager::UnregisterFromOnCheatManagerCreated(CheatManagerRegistrationHandle);

	Super::Deinitialize();
}

bool UHTeamSubsystem::RegisterTeamInfo(AHTeamInfoBase* TeamInfo)
{
	if (!ensure(TeamInfo))
	{
		return false;
	}

	const int32 TeamId = TeamInfo->GetTeamId();
	if (ensure(TeamId != INDEX_NONE))
	{
		FHTeamTrackingInfo& Entry = TeamMap.FindOrAdd(TeamId);
		Entry.SetTeamInfo(TeamInfo);

		return true;
	}

	return false;
}

bool UHTeamSubsystem::UnregisterTeamInfo(AHTeamInfoBase* TeamInfo)
{
	if (!ensure(TeamInfo))
	{
		return false;
	}

	const int32 TeamId = TeamInfo->GetTeamId();
	if (ensure(TeamId != INDEX_NONE))
	{
		FHTeamTrackingInfo* Entry = TeamMap.Find(TeamId);

		// If it couldn't find the entry, this is probably a leftover actor from a previous world, ignore it
		if (Entry)
		{
			Entry->RemoveTeamInfo(TeamInfo);

			return true;
		}
	}

	return false;
}

bool UHTeamSubsystem::ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamIndex)
{
	const FGenericTeamId NewTeamID = IntegerToGenericTeamId(NewTeamIndex);
	if (AHPlayerState* HPS = const_cast<AHPlayerState*>(FindPlayerStateFromActor(ActorToChange)))
	{
		HPS->SetGenericTeamId(NewTeamID);
		return true;
	}
	else if (IHTeamAgentInterface* TeamActor = Cast<IHTeamAgentInterface>(ActorToChange))
	{
		TeamActor->SetGenericTeamId(NewTeamID);
		return true;
	}
	else
	{
		return false;
	}
}

int32 UHTeamSubsystem::FindTeamFromObject(const UObject* TestObject) const
{
	// See if it's directly a team agent
	if (const IHTeamAgentInterface* ObjectWithTeamInterface = Cast<IHTeamAgentInterface>(TestObject))
	{
		return GenericTeamIdToInteger(ObjectWithTeamInterface->GetGenericTeamId());
	}

	if (const AActor* TestActor = Cast<const AActor>(TestObject))
	{
		// See if the instigator is a team actor
		if (const IHTeamAgentInterface* InstigatorWithTeamInterface = Cast<IHTeamAgentInterface>(TestActor->GetInstigator()))
		{
			return GenericTeamIdToInteger(InstigatorWithTeamInterface->GetGenericTeamId());
		}

		// TeamInfo actors don't actually have the team interface, so they need a special case
		if (const AHTeamInfoBase* TeamInfo = Cast<AHTeamInfoBase>(TestActor))
		{
			return TeamInfo->GetTeamId();
		}

		// Fall back to finding the associated player state
		if (const AHPlayerState* HPS = FindPlayerStateFromActor(TestActor))
		{
			return HPS->GetTeamId();
		}
	}

	return INDEX_NONE;
}

const AHPlayerState* UHTeamSubsystem::FindPlayerStateFromActor(const AActor* PossibleTeamActor) const
{
	if (PossibleTeamActor != nullptr)
	{
		if (const APawn* Pawn = Cast<const APawn>(PossibleTeamActor))
		{
			//@TODO: Consider an interface instead or have team actors register with the subsystem and have it maintain a map? (or LWC style)
			if (AHPlayerState* HPS = Pawn->GetPlayerState<AHPlayerState>())
			{
				return HPS;
			}
		}
		else if (const AController* PC = Cast<const AController>(PossibleTeamActor))
		{
			if (AHPlayerState* HPS = Cast<AHPlayerState>(PC->PlayerState))
			{
				return HPS;
			}
		}
		else if (const AHPlayerState* HPS = Cast<const AHPlayerState>(PossibleTeamActor))
		{
			return HPS;
		}

		// Try the instigator
// 		if (AActor* Instigator = PossibleTeamActor->GetInstigator())
// 		{
// 			if (ensure(Instigator != PossibleTeamActor))
// 			{
// 				return FindPlayerStateFromActor(Instigator);
// 			}
// 		}
	}

	return nullptr;
}

EHTeamComparison UHTeamSubsystem::CompareTeams(const UObject* A, const UObject* B, int32& TeamIdA, int32& TeamIdB) const
{
	TeamIdA = FindTeamFromObject(Cast<const AActor>(A));
	TeamIdB = FindTeamFromObject(Cast<const AActor>(B));

	if ((TeamIdA == INDEX_NONE) || (TeamIdB == INDEX_NONE))
	{
		return EHTeamComparison::InvalidArgument;
	}
	else
	{
		return (TeamIdA == TeamIdB) ? EHTeamComparison::OnSameTeam : EHTeamComparison::DifferentTeams;
	}
}

EHTeamComparison UHTeamSubsystem::CompareTeams(const UObject* A, const UObject* B) const
{
	int32 TeamIdA;
	int32 TeamIdB;
	return CompareTeams(A, B, /*out*/ TeamIdA, /*out*/ TeamIdB);
}

void UHTeamSubsystem::FindTeamFromActor(const UObject* TestObject, bool& bIsPartOfTeam, int32& TeamId) const
{
	TeamId = FindTeamFromObject(TestObject);
	bIsPartOfTeam = TeamId != INDEX_NONE;
}

void UHTeamSubsystem::AddTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler = [&](const FString& ErrorMessage)
	{
		UE_LOG(LogHTeams, Error, TEXT("AddTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
	};

	if (FHTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.AddStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

void UHTeamSubsystem::RemoveTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler = [&](const FString& ErrorMessage)
	{
		UE_LOG(LogHTeams, Error, TEXT("RemoveTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
	};

	if (FHTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.RemoveStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

int32 UHTeamSubsystem::GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const
{
	if (const FHTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		const int32 PublicStackCount = (Entry->PublicInfo != nullptr) ? Entry->PublicInfo->TeamTags.GetStackCount(Tag) : 0;
		const int32 PrivateStackCount = (Entry->PrivateInfo != nullptr) ? Entry->PrivateInfo->TeamTags.GetStackCount(Tag) : 0;
		return PublicStackCount + PrivateStackCount;
	}
	else
	{
		UE_LOG(LogHTeams, Verbose, TEXT("GetTeamTagStackCount(TeamId: %d, Tag: %s) failed because it was passed an unknown team id"), TeamId, *Tag.ToString());
		return 0;
	}
}

bool UHTeamSubsystem::TeamHasTag(int32 TeamId, FGameplayTag Tag) const
{
	return GetTeamTagStackCount(TeamId, Tag) > 0;
}

bool UHTeamSubsystem::DoesTeamExist(int32 TeamId) const
{
	return TeamMap.Contains(TeamId);
}

TArray<int32> UHTeamSubsystem::GetTeamIDs() const
{
	TArray<int32> Result;
	TeamMap.GenerateKeyArray(Result);
	Result.Sort();
	return Result;
}

bool UHTeamSubsystem::CanCauseDamage(const UObject* Instigator, const UObject* Target, bool bAllowDamageToSelf) const
{
	if (bAllowDamageToSelf)
	{
		if ((Instigator == Target) || (FindPlayerStateFromActor(Cast<AActor>(Instigator)) == FindPlayerStateFromActor(Cast<AActor>(Target))))
		{
			return true;
		}
	}

	int32 InstigatorTeamId;
	int32 TargetTeamId;
	const EHTeamComparison Relationship = CompareTeams(Instigator, Target, /*out*/ InstigatorTeamId, /*out*/ TargetTeamId);
	if (Relationship == EHTeamComparison::DifferentTeams)
	{
		return true;
	}
	else if ((Relationship == EHTeamComparison::InvalidArgument) && (InstigatorTeamId != INDEX_NONE))
	{
		// Allow damaging non-team actors for now, as long as they have an ability system component
		//@TODO: This is temporary until the target practice dummy has a team assignment
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Cast<const AActor>(Target)) != nullptr;
	}

	return false;
}

UHTeamDisplayAsset* UHTeamSubsystem::GetTeamDisplayAsset(int32 TeamId, int32 ViewerTeamId)
{
	// Currently ignoring ViewerTeamId

	if (FHTeamTrackingInfo* Entry = TeamMap.Find(TeamId))
	{
		return Entry->DisplayAsset;
	}

	return nullptr;
}

UHTeamDisplayAsset* UHTeamSubsystem::GetEffectiveTeamDisplayAsset(int32 TeamId, UObject* ViewerTeamAgent)
{
	return GetTeamDisplayAsset(TeamId, FindTeamFromObject(ViewerTeamAgent));
}

void UHTeamSubsystem::NotifyTeamDisplayAssetModified(UHTeamDisplayAsset* /*ModifiedAsset*/)
{
	// Broadcasting to all observers when a display asset is edited right now, instead of only the edited one
	for (const auto& KVP : TeamMap)
	{
		const int32 TeamId = KVP.Key;
		const FHTeamTrackingInfo& TrackingInfo = KVP.Value;

		TrackingInfo.OnTeamDisplayAssetChanged.Broadcast(TrackingInfo.DisplayAsset);
	}
}

FOnHTeamDisplayAssetChangedDelegate& UHTeamSubsystem::GetTeamDisplayAssetChangedDelegate(int32 TeamId)
{
	return TeamMap.FindOrAdd(TeamId).OnTeamDisplayAssetChanged;
}
