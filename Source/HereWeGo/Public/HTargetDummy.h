// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HTargetDummy.generated.h"

class UHWorldUserWidget;
class UHHealthComponent;
class UHGASComponent;

UCLASS()
class HEREWEGO_API AHTargetDummy : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	UFUNCTION()
	void OnHealthChanged(UHHealthComponent* AffectedHealthComp, float OldValue, float NewValue, AActor* InstigatorActor);

	// Sets default values for this actor's properties
	AHTargetDummy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UHWorldUserWidget> ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(VisibleAnywhere)
	//TObjectPtr<UHGASComponent> GASComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHHealthComponent> HealthComp;
};
