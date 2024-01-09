// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HPenetrationAvoidanceFeeler.generated.h"

/**
 * Struct defining a feeler ray used for camera penetration avoidance.
 */
USTRUCT()
struct FHPenetrationAvoidanceFeeler
{
	GENERATED_BODY()

	/** FRotator describing deviance from main ray */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	FRotator AdjustmentRot;

	/** how much this feeler affects the final position if it hits the world */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float WorldWeight;

	/** how much this feeler affects the final position if it hits a APawn (setting to 0 will not attempt to collide with pawns at all) */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float PawnWeight;

	/** extent to use for collision when tracing this feeler */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	float Extent;

	/** minimum frame interval between traces with this feeler if nothing was hit last frame */
	UPROPERTY(EditAnywhere, Category=PenetrationAvoidanceFeeler)
	int32 TraceInterval;

	/** number of frames since this feeler was used */
	UPROPERTY(transient)
	int32 FramesUntilNextTrace;


	FHPenetrationAvoidanceFeeler()
		: AdjustmentRot(ForceInit)
		, WorldWeight(0)
		, PawnWeight(0)
		, Extent(0)
		, TraceInterval(0)
		, FramesUntilNextTrace(0)
	{
	}

	FHPenetrationAvoidanceFeeler(const FRotator& InAdjustmentRot,
									const float& InWorldWeight, 
									const float& InPawnWeight, 
									const float& InExtent, 
									const int32& InTraceInterval = 0, 
									const int32& InFramesUntilNextTrace = 0)
		: AdjustmentRot(InAdjustmentRot)
		, WorldWeight(InWorldWeight)
		, PawnWeight(InPawnWeight)
		, Extent(InExtent)
		, TraceInterval(InTraceInterval)
		, FramesUntilNextTrace(InFramesUntilNextTrace)
	{
	}
};
