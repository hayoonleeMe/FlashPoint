// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "FPGameplayTags.h"
#include "System/FPAssetManager.h"
#include "System/FPGameInstance.h"

void ALobbyGameMode::TravelToMatchLevel(EMatchMode MatchMode) const
{
	// 로비에서 Seamless Travel을 수행하기 전, MatchInfo 데이터 캐싱
	CacheMatchInfo();
	
	FString URL;	
	if (MatchMode == EMatchMode::TeamDeathMatch)
	{
		URL = UFPAssetManager::GetAssetPathByTag(FPGameplayTags::Asset::Level::Warehouse_TDM).GetAssetName();
	}
	else if (MatchMode == EMatchMode::FreeForAll)
	{
		URL = UFPAssetManager::GetAssetPathByTag(FPGameplayTags::Asset::Level::Warehouse_FFA).GetAssetName();
	}

	if (!URL.IsEmpty() && GetWorld())
	{
		GetWorld()->ServerTravel(URL);
	}
}
