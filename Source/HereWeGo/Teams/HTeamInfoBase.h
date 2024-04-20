// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HGameplayTagStackContainer.h"
#include "HTeamInfoBase.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UHTeamCreationComponent;
class UHTeamSubsystem;
class UObject;
struct FFrame;

UCLASS(Abstract)
class AHTeamInfoBase : public AInfo
{
	GENERATED_BODY()

public:
	AHTeamInfoBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	int32 GetTeamId() const { return TeamId; }

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

protected:
	virtual void RegisterWithTeamSubsystem(UHTeamSubsystem* Subsystem);
	void TryRegisterWithTeamSubsystem();

private:
	void SetTeamId(int32 NewTeamId);

	UFUNCTION()
	void OnRep_TeamId();

public:
	friend UHTeamCreationComponent;

	UPROPERTY(Replicated)
	FHGameplayTagStackContainer TeamTags;

private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamId)
	int32 TeamId;
};