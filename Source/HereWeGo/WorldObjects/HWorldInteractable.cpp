// Fill out your copyright notice in the Description page of Project Settings.


#include "HWorldInteractable.h"

// Sets default values
AHWorldInteractable::AHWorldInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AHWorldInteractable::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
	FInteractionOptionBuilder& InteractionBuilder)
{
	InteractionBuilder.AddInteractionOption(Option);
}



