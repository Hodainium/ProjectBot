// Fill out your copyright notice in the Description page of Project Settings.


#include "HPlayerController.h"

#include "HAbilitySystemComponent.h"
#include "HPlayerCameraManager.h"
#include "HPlayerState.h"
#include "Blueprint/UserWidget.h"

AHPlayerController::AHPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AHPlayerCameraManager::StaticClass();
}

AHPlayerState* AHPlayerController::GetHPlayerState() const
{
	return CastChecked<AHPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UHAbilitySystemComponent* AHPlayerController::GetHAbilitySystemComponent() const
{
	const AHPlayerState* PS = GetHPlayerState();
	return (PS ? PS->GetHAbilitySystemComp() : nullptr);
}

void AHPlayerController::CreateHUD()
{
	if(HUDWidgetInstance)
	{
		return;
	}

	if(!HUDWidgetClassType)
	{
		UE_LOG(LogTemp, Error, TEXT("There is no default hud class type set! Cannot create HUD instance for controller"));
		return;
	}

	if(!IsLocalPlayerController())
	{
		return;
	}

	AHPlayerState* HPlayerState = GetPlayerState<AHPlayerState>();
	if(!HPlayerState)
	{
		return;
	}

	HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClassType);
	HUDWidgetInstance->AddToViewport();

	//todo initialize hud with attributes here

}

void AHPlayerController::OnCameraPenetratingTarget()
{
	bHideViewTargetPawnNextFrame = true;
}

void AHPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	/*if (UHAbilitySystemComponent* ASC = GetHAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}*/

	if (AbilitySystemComponentRef.IsValid())
	{
		AbilitySystemComponentRef->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AHPlayerController::RegisterASCRef(UHAbilitySystemComponent* Ref)
{
	AbilitySystemComponentRef = Ref;
}

void AHPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	BroadcastOnPlayerStateChanged();
}

void AHPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	BroadcastOnPlayerStateChanged();
}

void AHPlayerController::UpdateHiddenComponents(const FVector& ViewLocation,
                                                TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

	if (bHideViewTargetPawnNextFrame)
	{
		AActor* const ViewTargetPawn = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;
		if (ViewTargetPawn)
		{
			// internal helper func to hide all the components
			auto AddToHiddenComponents = [&OutHiddenComponents](const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
			{
				// add every component and all attached children
				for (UPrimitiveComponent* Comp : InComponents)
				{
					if (Comp->IsRegistered())
					{
						OutHiddenComponents.Add(Comp->ComponentId);

						for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
						{
							static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));
							UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);
							if (AttachChildPC && AttachChildPC->IsRegistered() && !AttachChildPC->ComponentTags.Contains(NAME_NoParentAutoHide))
							{
								OutHiddenComponents.Add(AttachChildPC->ComponentId);
							}
						}
					}
				}
			};

			//TODO Solve with an interface.  Gather hidden components or something.
			//TODO Hiding isn't awesome, sometimes you want the effect of a fade out over a proximity, needs to bubble up to designers.

			// hide pawn's components
			TInlineComponentArray<UPrimitiveComponent*> PawnComponents;
			ViewTargetPawn->GetComponents(PawnComponents);
			AddToHiddenComponents(PawnComponents);

			//// hide weapon too
			//if (ViewTargetPawn->CurrentWeapon)
			//{
			//	TInlineComponentArray<UPrimitiveComponent*> WeaponComponents;
			//	ViewTargetPawn->CurrentWeapon->GetComponents(WeaponComponents);
			//	AddToHiddenComponents(WeaponComponents);
			//}
		}

		// we consumed it, reset for next frame
		bHideViewTargetPawnNextFrame = false;
	}
}

void AHPlayerController::BroadcastOnPlayerStateChanged()
{
	//SHould do something
}

void AHPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BroadcastOnPlayerStateChanged();
	//In case Playerstate is repped before possession
	//CreateHUD();

}


