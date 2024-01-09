// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HWorldUserWidget.generated.h"

class USizeBox;

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox;

	UPROPERTY()
	FWidgetTransform SizeBoxTransform;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	

	//virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	UHWorldUserWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;

	UPROPERTY(EditAnywhere, Category = "UI")
	float DistanceScalingThreshold;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (ExposeOnSpawn = true))
	TObjectPtr<AActor> AttachedActor;

	
	
};
