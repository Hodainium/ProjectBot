// Fill out your copyright notice in the Description page of Project Settings.


#include "HPlayerController.h"

#include "CommonUIExtensions.h"
#include "HAbilitySystemComponent.h"
#include "HLogChannels.h"
#include "HPlayerCameraManager.h"
#include "HPlayerCharacter.h"
#include "HPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "HereWeGo/Actors/Characters/HCharacterBase.h"
#include "HereWeGo/Tags/H_Tags.h"
#include "HereWeGo/UI/HHUDLayout.h"
#include "Logging/StructuredLog.h"

AHPlayerController::AHPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AHPlayerCameraManager::StaticClass();
}

AHPlayerState* AHPlayerController::GetHPlayerState() const
{
	return CastChecked<AHPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

AHCharacterBase* AHPlayerController::GetHCharacterBase() const
{
	return CastChecked<AHCharacterBase>(GetPawn(), ECastCheckedType::NullAllowed);
}


UHAbilitySystemComponent* AHPlayerController::GetHAbilitySystemComponent() const
{
	const AHCharacterBase* CharBase = GetHCharacterBase();
	return (CharBase ? CharBase->GetHAbilitySystemComponent() : nullptr);
}

UAbilitySystemComponent* AHPlayerController::GetAbilitySystemComponent() const
{
	const AHCharacterBase* CharBase = GetHCharacterBase();
	return (CharBase ? CharBase->GetAbilitySystemComponent() : nullptr);
}

void AHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::HandlePossessedPawnChanged);
}

void AHPlayerController::HandlePossessedPawnChanged(APawn* OldPawnBroadcasted, APawn* NewPawnBroadcasted)
{
	if(!HUDLayoutWidget.IsValid())
	{
		CreateHUD();
	}
	//Eventually have logic to switch huds based on pawndata
	//Will require calling removeHUD
}

void AHPlayerController::CreateHUD()
{
	// Add HUD Layout widget to the player's Game UI Layer
	if (IsLocalPlayerController())
	{
		UE_LOG(LogHGame, Warning, TEXT("Pushing Game HUD [%s] to UI"), *GetNameSafe(HUDLayoutClass));
		HUDLayoutWidget = UCommonUIExtensions::PushContentToLayer_ForPlayer(GetLocalPlayer(), H_CommonUI_Tags::TAG_UI_LAYER_GAME, HUDLayoutClass);
	}
}

void AHPlayerController::RemoveHUD()
{
	// Remove any HUD we added to the player's UI
	if (HUDLayoutWidget.IsValid())
	{
		UE_LOGFMT(LogHGame, Warning, "Cleaning up HUD Layout Widget");
		UCommonUIExtensions::PopContentFromLayer(HUDLayoutWidget.Get());
		HUDLayoutWidget.Reset();
	}
}

void AHPlayerController::OnCameraPenetratingTarget()
{
	bHideViewTargetPawnNextFrame = true;
}

void AHPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UHAbilitySystemComponent* ASC = GetHAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
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


