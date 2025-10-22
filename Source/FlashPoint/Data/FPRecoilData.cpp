// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/FPRecoilData.h"

#include "FPGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPRecoilData)

TMap<FGameplayTag, FGameplayTag> UFPRecoilData::WeaponTypeToRecoilDataTagMap =
{
	{ FPGameplayTags::Weapon::Type::Pistol, FPGameplayTags::Asset::RecoilData::Pistol },
	{ FPGameplayTags::Weapon::Type::Rifle, FPGameplayTags::Asset::RecoilData::Rifle },
	{ FPGameplayTags::Weapon::Type::Shotgun, FPGameplayTags::Asset::RecoilData::Shotgun },
	{ FPGameplayTags::Weapon::Type::SMG, FPGameplayTags::Asset::RecoilData::SMG },
	{ FPGameplayTags::Weapon::Type::SniperRifle, FPGameplayTags::Asset::RecoilData::SniperRifle }
};

FGameplayTag UFPRecoilData::GetRecoilDataTagByWeaponType(const FGameplayTag& WeaponTypeTag)
{
	return WeaponTypeToRecoilDataTagMap.FindRef(WeaponTypeTag);
}
