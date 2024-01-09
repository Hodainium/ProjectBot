// Fill out your copyright notice in the Description page of Project Settings.


#include "HGameData.h"

#include "HereWeGo/HAssetManager.h"

UHGameData::UHGameData()
{
}

const UHGameData& UHGameData::UHGameData::Get()
{
	return UHAssetManager::Get().GetGameData();
}