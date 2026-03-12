// Fill out your copyright notice in the Description page of Project Settings.


#include "FPWeaponConfigData.h"

#include "FPGameplayTags.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPWeaponConfigData)

UFPWeaponConfigData* UFPWeaponConfigData::Get(const FGameplayTag& WeaponTypeTag)
{
	return UFPAssetManager::GetAssetByTag<UFPWeaponConfigData>(FPGameplayTags::Asset::WeaponConfigData, WeaponTypeTag);
}

UFPWeaponConfigData::UFPWeaponConfigData()
{
	LeftHandAttachSocketName = TEXT("LeftHandAttach");
	AimDownSightSocketName = TEXT("AimDownSight");
	FirstPersonDefaultWeaponFOV = 90.f;
	AimDownSightFOV = 70.f;
	AimDownSightWeaponFOV = 70.f;
}
