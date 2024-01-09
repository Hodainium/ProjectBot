// Fill out your copyright notice in the Description page of Project Settings.


#include "HWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

UHWorldUserWidget::UHWorldUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DistanceScalingThreshold = 900.f;
	//CachedTransform = ParentSizeBox->GetRenderTransform();
}

void UHWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!IsValid(AttachedActor))
	{
		RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("Widget: AttachedActor is invalid. Removing widget"));
		return;
	}

	FVector2D ScreenPosition;
	FVector UIWorldPosition;
	

	//bool IsOnScreen = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition, false);
	bool IsOnScreen = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPositionWithDistance(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, UIWorldPosition, false);

	if (IsOnScreen)
	{
		if (ParentSizeBox)
		{
			SizeBoxTransform = ParentSizeBox->GetRenderTransform();

			if (UIWorldPosition.Z >= DistanceScalingThreshold)
			{
				float NewScaleFactor = (DistanceScalingThreshold / UIWorldPosition.Z);

				ParentSizeBox->SetRenderScale(FVector2D::One() * NewScaleFactor);
				
			}
			else
			{
				ParentSizeBox->SetRenderScale(FVector2D::One());
			}

			ScreenPosition = static_cast<FVector2D>(UIWorldPosition); 
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}

	if (ParentSizeBox)
	{
		ParentSizeBox->SetVisibility(IsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}




