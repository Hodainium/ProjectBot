// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "HGameplayTagStackContainer.h"
#include "HInventoryInterface.h"
#include "HVerbMessage.h"
#include "HWeaponInterface.h"
#include "ModularPlayerState.h"
#include "HereWeGo/Interfaces/HTeamAgentInterface.h"
#include "HPlayerState.generated.h"

class AHPlayerController;
class UHPawnData;
class UAbilitySystemComponent;
class UHAbilitySystemComponent;
class UHWeaponComponent;
class UHAttributeSetBase;
struct FGameplayEffectSpec;

/** Defines the types of client connected */
UENUM()
enum class EHPlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

/**
 * AHPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(Config = Game)
class HEREWEGO_API AHPlayerState : public AModularPlayerState, public IHTeamAgentInterface //, public IAbilitySystemInterface, 
{
	GENERATED_BODY()

public:
	AHPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	AHPlayerController* GetHPlayerController() const;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UHPawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface

	//~IHTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FHOnTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IHTeamAgentInterface interface

	static const FName NAME_HAbilityReady;

	void SetPlayerConnectionType(EHPlayerConnectionType NewType);
	EHPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	/** Returns the Squad ID of the squad the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetSquadId() const
	{
		return MySquadID;
	}

	/** Returns the Team ID of the team the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() const
	{
		return GenericTeamIdToInteger(MyTeamID);
	}

	void SetSquadID(int32 NewSquadID);

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category = Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category = Teams)
	bool HasStatTag(FGameplayTag Tag) const;

	// Send a message to just this player
	// (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "H|PlayerState")
	void ClientBroadcastMessage(const FHVerbMessage Message);

	// Gets the replicated view rotation of this player, used for spectating
	FRotator GetReplicatedViewRotation() const;

	// Sets the replicated view rotation, only valid on the server
	void SetReplicatedViewRotation(const FRotator& NewRotation);

private:
	//void OnExperienceLoaded(const UHExperienceDefinition* CurrentExperience);

protected:
	UFUNCTION()
	void OnRep_PawnData();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UHPawnData> PawnData;

private:

	//// The ability system component sub-object used by player characters.
	//UPROPERTY(VisibleAnywhere, Category = "H|PlayerState")
	//TObjectPtr<UHAbilitySystemComponent> AbilitySystemComponent;

	//// Health attribute set used by this actor.
	//UPROPERTY()
	//TObjectPtr<const class UHHealthSet> HealthSet;
	//// Combat attribute set used by this actor.
	//UPROPERTY()
	//TObjectPtr<const class UHCombatSet> CombatSet;

	UPROPERTY(Replicated)
	EHPlayerConnectionType MyPlayerConnectionType;

	UPROPERTY()
	FHOnTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY(ReplicatedUsing = OnRep_MySquadID)
	int32 MySquadID;

	UPROPERTY(Replicated)
	FHGameplayTagStackContainer StatTags;

	UPROPERTY(Replicated)
	FRotator ReplicatedViewRotation;

private:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	UFUNCTION()
	void OnRep_MySquadID();
};
