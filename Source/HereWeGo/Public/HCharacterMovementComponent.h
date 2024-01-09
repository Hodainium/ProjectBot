// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HCharacterMovementComponent.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FHCharacterGroundInfo
{
	GENERATED_BODY()

	FHCharacterGroundInfo() : LastUpdateFrame(0), GroundDistance(0.f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

UCLASS()
class HEREWEGO_API UHCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FHSavedMove;

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const override;

	virtual bool DoJump(bool bReplayingMoves) override;

	float AddedYVelocityFromForwardSpeed(float Multiplier, float AdditionalJumpVelocity = 0.f);

	float GetForwardVelocity();

public:

	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	UFUNCTION(BlueprintCallable, Category="Movement|Sprint")
	void StartSprinting();

	UFUNCTION(BlueprintCallable, Category = "Movement|Sprint")
	void StopSprinting();

	UFUNCTION(BlueprintCallable, Category = "Movement|Sprint")
	bool GetIsRequestingSprint() const;

	UFUNCTION(BlueprintCallable, Category = "Movement|ADS")
	void StartAimDownSights();

	UFUNCTION(BlueprintCallable, Category = "Movement|ADS")
	void StopAimDownSights();

	UFUNCTION(BlueprintCallable, Category = "Movement|GroundInfo")
	const FHCharacterGroundInfo& GetGroundInfo();

protected:
	UPROPERTY(EditAnywhere, Category = "Movement|Sprint")
	float RunSpeedMultiplier = 1.7f;

	UPROPERTY(EditAnywhere, Category = "Movement|Sprint")
	float ADSSpeedMultiplier = 0.9f;

	FHCharacterGroundInfo CachedGroundInfo;

private:
	//Custom flags
	uint8 RequestToStartSprinting : 1;
	uint8 RequestToStartADS : 1;
	
};

class FHSavedMove : public FSavedMove_Character
{
public:

	typedef FSavedMove_Character Super;

	virtual void Clear() override;

	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;

	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void PrepMoveFor(ACharacter* Character) override;

private:
	uint8 SavedRequestToStartSprinting : 1;
	uint8 SavedRequestToStartADS : 1;
};

class FHNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
public:

	typedef FNetworkPredictionData_Client_Character Super;

	FHNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};
