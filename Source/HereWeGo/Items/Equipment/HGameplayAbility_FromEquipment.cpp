// Copyright Epic Games, Inc. All Rights Reserved.

#include "HGameplayAbility_FromEquipment.h"
#include "HEquipmentInstance.h"
#include "HInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HGameplayAbility_FromEquipment)

UHGameplayAbility_FromEquipment::UHGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UHEquipmentInstance* UHGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UHEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UHInventoryItemInstance* UHGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UHEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UHInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UHGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(NSLOCTEXT("H", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
