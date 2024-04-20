// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponStateComponent.h"

#include "HEquipmentComponent.h"
#include "HereWeGo/Physics/PhysicalMaterialWithTags.h"
#include "HereWeGo/Tags/H_Tags.h"
#include "Instances/HRangedWeaponInstance.h"
#include "Kismet/GameplayStatics.h"


UHWeaponStateComponent::UHWeaponStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UHWeaponStateComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (UHEquipmentComponent* EquipmentManager = Pawn->FindComponentByClass<UHEquipmentComponent>())
		{
			if (UHRangedWeaponInstance* CurrentWeapon = Cast<UHRangedWeaponInstance>(EquipmentManager->GetFirstInstanceOfType(UHRangedWeaponInstance::StaticClass())))
			{
				CurrentWeapon->Tick(DeltaTime);
			}
		}
	}
}

bool UHWeaponStateComponent::ShouldShowHitAsSuccess(const FHitResult& Hit) const
{
	AActor* HitActor = Hit.GetActor();

	////@TODO: Don't treat a hit that dealt no damage (due to invulnerability or similar) as a success
	//UWorld* World = GetWorld();
	//if (UHTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UHTeamSubsystem>(GetWorld()))
	//{
	//	return TeamSubsystem->CanCauseDamage(GetController<APlayerController>(), Hit.GetActor());
	//}

	return true;
	return false;
}

bool UHWeaponStateComponent::ShouldUpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext) const
{
	//@TODO: Implement me, for the purposes of this component we really only care about damage caused by a weapon
	// or projectile fired from a weapon, and should filter to that
	// (or perhaps see if the causer is also the source of our active reticle config)
	return EffectContext.GetEffectCauser() != nullptr;
}

void UHWeaponStateComponent::ClientConfirmTargetData_Implementation(uint16 UniqueId, bool bSuccess, const TArray<uint8>& HitReplaces)
{
	for (int i = 0; i < UnconfirmedServerSideHitMarkers.Num(); i++)
	{
		FHServerSideHitMarkerBatch& Batch = UnconfirmedServerSideHitMarkers[i];
		if (Batch.UniqueId == UniqueId)
		{
			if (bSuccess && (HitReplaces.Num() != Batch.Markers.Num()))
			{
				UWorld* World = GetWorld();
				bool bFoundShowAsSuccessHit = false;

				int32 HitLocationIndex = 0;
				for (const FHScreenSpaceHitLocation& Entry : Batch.Markers)
				{
					if (!HitReplaces.Contains(HitLocationIndex) && Entry.bShowAsSuccess)
					{
						// Only need to do this once
						if (!bFoundShowAsSuccessHit)
						{
							ActuallyUpdateDamageInstigatedTime();
						}

						bFoundShowAsSuccessHit = true;

						LastWeaponDamageScreenLocations.Add(Entry);
					}
					++HitLocationIndex;
				}
			}

			UnconfirmedServerSideHitMarkers.RemoveAt(i);
			break;
		}
	}
}

void UHWeaponStateComponent::AddUnconfirmedServerSideHitMarkers(const FGameplayAbilityTargetDataHandle& InTargetData, const TArray<FHitResult>& FoundHits)
{
	FHServerSideHitMarkerBatch& NewUnconfirmedHitMarker = UnconfirmedServerSideHitMarkers.Emplace_GetRef(InTargetData.UniqueId);

	if (APlayerController* OwnerPC = GetController<APlayerController>())
	{
		for (const FHitResult& Hit : FoundHits)
		{
			FVector2D HitScreenLocation;
			if (UGameplayStatics::ProjectWorldToScreen(OwnerPC, Hit.Location, /*out*/ HitScreenLocation, /*bPlayerViewportRelative=*/ false))
			{
				FHScreenSpaceHitLocation& Entry = NewUnconfirmedHitMarker.Markers.AddDefaulted_GetRef();
				Entry.Location = HitScreenLocation;
				Entry.bShowAsSuccess = ShouldShowHitAsSuccess(Hit);

				// Determine the hit zone
				//FGameplayTag HitZone;
				if (const UPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UPhysicalMaterialWithTags>(Hit.PhysMaterial.Get()))
				{
					for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
					{
						if (MaterialTag.MatchesTag(H_Material_Tags::TAG_Gameplay_Zone))
						{
							Entry.HitZone = MaterialTag;
							break;
						}
					}
				}
			}
		}
	}
}

void UHWeaponStateComponent::UpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext)
{
	if (ShouldUpdateDamageInstigatedTime(EffectContext))
	{
		ActuallyUpdateDamageInstigatedTime();
	}
}

void UHWeaponStateComponent::ActuallyUpdateDamageInstigatedTime()
{
	// If our LastWeaponDamageInstigatedTime was not very recent, clear our LastWeaponDamageScreenLocations array
	UWorld* World = GetWorld();
	if (World->GetTimeSeconds() - LastWeaponDamageInstigatedTime > 0.1)
	{
		LastWeaponDamageScreenLocations.Reset();
	}
	LastWeaponDamageInstigatedTime = World->GetTimeSeconds();
}

double UHWeaponStateComponent::GetTimeSinceLastHitNotification() const
{
	UWorld* World = GetWorld();
	return World->TimeSince(LastWeaponDamageInstigatedTime);
}

