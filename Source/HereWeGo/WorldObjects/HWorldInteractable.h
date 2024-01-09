// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HereWeGo/Interaction/IInteractableTarget.h"
#include "HereWeGo/Interaction/InteractionOption.h"
#include "HWorldInteractable.generated.h"

UCLASS()
class HEREWEGO_API AHWorldInteractable : public AActor, public IInteractableTarget
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHWorldInteractable();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder) override;

protected:

	UPROPERTY(EditAnywhere)
	FInteractionOption Option;

};
