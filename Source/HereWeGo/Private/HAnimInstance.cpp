// Fill out your copyright notice in the Description page of Project Settings.


#include "HAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "HCharacterMovementComponent.h"
#include "HPlayerCharacter.h"
#include "Misc/DataValidation.h"

UHAnimInstance::UHAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);



	UE_LOG(LogTemp, Warning, TEXT("Done asc!"));
}

#if WITH_EDITOR
EDataValidationResult UHAnimInstance::IsDataValid(FDataValidationContext& Context)
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif

void UHAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//UE_LOG(LogTemp, Warning, TEXT("Searching!"));

	if(AActor* OwningActor = GetOwningActor())
	{
		if(UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor)) //Check for ASC normally
		{
			//UE_LOG(LogTemp, Warning, TEXT("Found asc!"));
			InitializeWithAbilitySystem(ASC);
		}
		else if(AHCharacterBase* HChar = Cast<AHCharacterBase>(OwningActor)) 
		{
			//If we couldnt find asc the normal way see if animinstance is attached to player if so bind to event for when playerstate is created
			//HChar->OnAscReady.BindUObject(this, &UHAnimInstance::InitializeWithAbilitySystem);
			//UE_LOG(LogTemp, Warning, TEXT("Trying to subscribe!"));
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Says we're done!"));
}

void UHAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const AHCharacterBase* Character = Cast<AHCharacterBase>(GetOwningActor());

	if(!Character)
	{
		return;
	}

	UHCharacterMovementComponent* CharMovementComponent = CastChecked<UHCharacterMovementComponent>(Character->GetCharacterMovement());

	const FHCharacterGroundInfo& GroundInfo = CharMovementComponent->GetGroundInfo();

	GroundDistance = GroundInfo.GroundDistance;
}
