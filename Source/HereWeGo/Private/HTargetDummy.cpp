// Fill out your copyright notice in the Description page of Project Settings.


#include "HTargetDummy.h"

#include "HHealthComponent.h"
#include "HWorldUserWidget.h"
#include "Blueprint/UserWidget.h"

void AHTargetDummy::OnDeathStarted(AActor* OwningActor)
{
}

void AHTargetDummy::OnDeathFinished(AActor* OwningActor)
{
}

void AHTargetDummy::OnHealthChanged(UHHealthComponent* AffectedHealthComp, float OldValue, float NewValue, AActor* InstigatorActor)
{
	if (NewValue <= OldValue)
	{
		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<UHWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}
	}
	
}

// Sets default values
AHTargetDummy::AHTargetDummy()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	//GASComponent = CreateDefaultSubobject<UHGASComponent>("GASComp");

	HealthComp = CreateDefaultSubobject<UHHealthComponent>("HealthComp");
	//HealthComp->RegisterWithGASComp(GASComponent); todo

	HealthComp->OnHealthChanged.AddDynamic(this, &ThisClass::OnHealthChanged);
	HealthComp->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComp->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
}

// Called when the game starts or when spawned
void AHTargetDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHTargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

