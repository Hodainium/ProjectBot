// Fill out your copyright notice in the Description page of Project Settings.


#include "HCharacterMovementComponent.h"

#include "HAttributeSetBase.h"
#include "HPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "GameFramework/PhysicsVolume.h"


namespace HCharacter
{
	static float GroundTraceDistance = 100000.f;
	FAutoConsoleVariableRef CVar_GroundTraceDistance(TEXT("HCharacter.GroundTraceDistance"), GroundTraceDistance, TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
}

void FHSavedMove::Clear()
{
	FSavedMove_Character::Clear();

	SavedRequestToStartSprinting = false;
	SavedRequestToStartADS = false;
}

uint8 FHSavedMove::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	if (SavedRequestToStartADS)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool FHSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
	float MaxDelta) const
{
	const FHSavedMove* HNewMove = static_cast<const FHSavedMove*>(NewMove.Get());

	if (SavedRequestToStartSprinting != HNewMove->SavedRequestToStartSprinting)
	{
		return false;
	}

	if(SavedRequestToStartADS != HNewMove->SavedRequestToStartADS)
	{
		return false;
	}

	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FHSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UHCharacterMovementComponent* CharMoveComponent = Cast<UHCharacterMovementComponent>(Character->GetCharacterMovement());

	if(CharMoveComponent)
	{
		SavedRequestToStartSprinting = CharMoveComponent->RequestToStartSprinting;
		SavedRequestToStartADS = CharMoveComponent->RequestToStartADS;
	}
}

void FHSavedMove::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);

	UHCharacterMovementComponent* CharMoveComponent = Cast<UHCharacterMovementComponent>(Character->GetCharacterMovement());

	if (CharMoveComponent)
	{
		CharMoveComponent->RequestToStartSprinting = SavedRequestToStartSprinting;
		CharMoveComponent->RequestToStartADS = SavedRequestToStartADS;
	}
}

FHNetworkPredictionData_Client_Character::FHNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FHNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FHSavedMove());
}


void UHCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UHCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

FVector UHCharacterMovementComponent::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity,
	float DeltaTime) const
{
	if (Cast<AHPlayerCharacter>(CharacterOwner)->bJumpAscending && !Cast<AHPlayerCharacter>(CharacterOwner)->bJumpInputHeldRaw && Velocity.Z >= 20.f)
	{
		return FVector(InitialVelocity.X, InitialVelocity.Y, InitialVelocity.Z / 4.f);
	}
	else
	{
		return Super::NewFallVelocity(InitialVelocity, Gravity, DeltaTime);
	}
}

bool UHCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		// Don't jump if we can't move up/down.
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Current velo is: %s"), *Velocity.ToString());
			float AddedJumpVelocity = AddedYVelocityFromForwardSpeed(0.25f);
			UE_LOG(LogTemp, Warning, TEXT("Added jump velo is: %f    From movespeed: %f"), AddedJumpVelocity, GetForwardVelocity())
			Velocity -= GetForwardVelocity() * CharacterOwner->GetActorForwardVector() * 0.2f; //in mario fspeed is * .8 so here we need to sub .2
			UE_LOG(LogTemp, Warning, TEXT("Movespeed is now: %f"), GetForwardVelocity());

			Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, JumpZVelocity+AddedJumpVelocity); //This is coming otu wrong :(
			SetMovementMode(MOVE_Falling);

			
			return true;
		}
	}

	return false;
}

float UHCharacterMovementComponent::AddedYVelocityFromForwardSpeed(float Multiplier, float AdditionalJumpVelocity)
{
	//dot product is magnitude of the projection of vectorA onto vectorB times the magnitude of VectorB
	if (Multiplier > 0.f)
	{
		return GetForwardVelocity() * Multiplier + AdditionalJumpVelocity;
	}
	else
	{
		return 0.f;
	}
	

}

float UHCharacterMovementComponent::GetForwardVelocity()
{
	return FVector::DotProduct(Velocity, CharacterOwner->GetActorForwardVector());
}

float UHCharacterMovementComponent::GetMaxSpeed() const //todo change speed to be linked to attribute set base 
{

	AHCharacterBase* Owner = Cast<AHCharacterBase>(GetOwner());
	if(!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s : Could not find owner in HMovecomp:Max Speed!"), *GetOwner()->GetActorNameOrLabel());
		return Super::GetMaxSpeed();
	}

	if(!Owner->IsAlive())
	{
		return 0.f;
	}

	if(RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * RunSpeedMultiplier;
	}

	if(RequestToStartADS && MovementMode != MOVE_Falling)
	{
		return Owner->GetMoveSpeed() * ADSSpeedMultiplier;
	}

	return Owner->GetMoveSpeed();

	/*switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		{
			if(RequestToStartSprinting)
			{
				return MaxWalkSpeed + AddedRunSpeed;
			}

			return IsCrouching() ? MaxWalkSpeedCrouched : MaxWalkSpeed;
		}
	default:
		{
		return Super::GetMaxSpeed();
		}
	}*/
}

void UHCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	RequestToStartADS = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* UHCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UHCharacterMovementComponent* MutableThis = const_cast<UHCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FHNetworkPredictionData_Client_Character(*this);

		/*MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;*/
	}

	return ClientPredictionData;
}

void UHCharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UHCharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

bool UHCharacterMovementComponent::GetIsRequestingSprint() const
{
	return RequestToStartSprinting;
}

void UHCharacterMovementComponent::StartAimDownSights()
{
	RequestToStartADS = true;
}

void UHCharacterMovementComponent::StopAimDownSights()
{
	RequestToStartADS = false;
}

const FHCharacterGroundInfo& UHCharacterMovementComponent::GetGroundInfo()
{
	if(!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}

	if (MovementMode == MOVE_Walking)
	{
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.f;
	}
	else
	{
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);

		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();

		const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);

		const FVector TraceStart(GetActorLocation());
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - HCharacter::GroundTraceDistance - CapsuleHalfHeight));

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(HCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParams;
		InitCollisionParams(QueryParams, ResponseParams);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParams);

		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = HCharacter::GroundTraceDistance;

		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.f);
		}
	}

	CachedGroundInfo.LastUpdateFrame = GFrameCounter;

	return CachedGroundInfo;
}








