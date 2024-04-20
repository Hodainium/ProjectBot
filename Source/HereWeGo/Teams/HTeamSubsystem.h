// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HTeamSubsystem.generated.h"


class AActor;
class AHPlayerState;
class AHTeamInfoBase;
class AHTeamPrivateInfo;
class AHTeamPublicInfo;
class FSubsystemCollectionBase;
class UHTeamDisplayAsset;
struct FFrame;
struct FGameplayTag;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHTeamDisplayAssetChangedDelegate, const UHTeamDisplayAsset*, DisplayAsset);

USTRUCT()
struct FHTeamTrackingInfo
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<AHTeamPublicInfo> PublicInfo = nullptr;

	UPROPERTY()
	TObjectPtr<AHTeamPrivateInfo> PrivateInfo = nullptr;

	UPROPERTY()
	TObjectPtr<UHTeamDisplayAsset> DisplayAsset = nullptr;

	UPROPERTY()
	FOnHTeamDisplayAssetChangedDelegate OnTeamDisplayAssetChanged;

public:
	void SetTeamInfo(AHTeamInfoBase* Info);
	void RemoveTeamInfo(AHTeamInfoBase* Info);
};

// Result of comparing the team affiliation for two actors
UENUM(BlueprintType)
enum class EHTeamComparison : uint8
{
	// Both actors are members of the same team
	OnSameTeam,

	// The actors are members of opposing teams
	DifferentTeams,

	// One (or both) of the actors was invalid or not part of any team
	InvalidArgument
};

/** A subsystem for easy access to team information for team-based actors (e.g., pawns or player states) */
UCLASS()
class HEREWEGO_API UHTeamSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UHTeamSubsystem();

	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	// Tries to registers a new team
	bool RegisterTeamInfo(AHTeamInfoBase* TeamInfo);

	// Tries to unregister a team, will return false if it didn't work
	bool UnregisterTeamInfo(AHTeamInfoBase* TeamInfo);

	// Changes the team associated with this actor if possible
	// Note: This function can only be called on the authority
	bool ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamId);

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	int32 FindTeamFromObject(const UObject* TestObject) const;

	// Returns the associated player state for this actor, or INDEX_NONE if it is not associated with a player
	const AHPlayerState* FindPlayerStateFromActor(const AActor* PossibleTeamActor) const;

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Teams, meta = (Keywords = "Get"))
	void FindTeamFromActor(const UObject* TestActor, bool& bIsPartOfTeam, int32& TeamId) const;

	// Compare the teams of two actors and returns a value indicating if they are on same teams, different teams, or one/both are invalid
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Teams, meta = (ExpandEnumAsExecs = ReturnValue))
	EHTeamComparison CompareTeams(const UObject* A, const UObject* B, int32& TeamIdA, int32& TeamIdB) const;

	// Compare the teams of two actors and returns a value indicating if they are on same teams, different teams, or one/both are invalid
	EHTeamComparison CompareTeams(const UObject* A, const UObject* B) const;

	// Returns true if the instigator can damage the target, taking into account the friendly fire settings
	bool CanCauseDamage(const UObject* Instigator, const UObject* Target, bool bAllowDamageToSelf = true) const;

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Teams)
	void AddTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Teams)
	void RemoveTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category = Teams)
	int32 GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category = Teams)
	bool TeamHasTag(int32 TeamId, FGameplayTag Tag) const;

	// Returns true if the specified team exists
	UFUNCTION(BlueprintCallable, Category = Teams)
	bool DoesTeamExist(int32 TeamId) const;

	// Gets the team display asset for the specified team, from the perspective of the specified team
	// (You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation)
	UFUNCTION(BlueprintCallable, Category = Teams)
	UHTeamDisplayAsset* GetTeamDisplayAsset(int32 TeamId, int32 ViewerTeamId);

	// Gets the team display asset for the specified team, from the perspective of the specified team
	// (You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation)
	UFUNCTION(BlueprintCallable, Category = Teams)
	UHTeamDisplayAsset* GetEffectiveTeamDisplayAsset(int32 TeamId, UObject* ViewerTeamAgent);

	// Gets the list of teams
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Teams)
	TArray<int32> GetTeamIDs() const;

	// Called when a team display asset has been edited, causes all team color observers to update
	void NotifyTeamDisplayAssetModified(UHTeamDisplayAsset* ModifiedAsset);

	// Register for a team display asset notification for the specified team ID
	FOnHTeamDisplayAssetChangedDelegate& GetTeamDisplayAssetChangedDelegate(int32 TeamId);

private:
	UPROPERTY()
	TMap<int32, FHTeamTrackingInfo> TeamMap;

	FDelegateHandle CheatManagerRegistrationHandle;
};

