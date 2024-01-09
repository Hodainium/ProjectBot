// Fill out your copyright notice in the Description page of Project Settings.


#include "HTagFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

FGameplayTag UHTagFunctionLibrary::GetTagFromString(const char* TagName)
{
	return UGameplayTagsManager::Get().RequestGameplayTag(FName(TagName));
}
