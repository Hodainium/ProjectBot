// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTabListWidgetBase.h"
#include "HereWeGo/UI/Buttons/HButtonBase.h"
#include "HTabButtonBase.generated.h"

class UCommonLazyImage;
/**
 * 
 */
UCLASS()
class HEREWEGO_API UHTabButtonBase : public UHButtonBase, public IHTabButtonInterface
{
	GENERATED_BODY()

public:

	void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
	void SetIconBrush(const FSlateBrush& Brush);

protected:

	UFUNCTION()
	virtual void SetTabLabelInfo_Implementation(const FHTabDescriptor& TabLabelInfo) override;

private:

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
};
