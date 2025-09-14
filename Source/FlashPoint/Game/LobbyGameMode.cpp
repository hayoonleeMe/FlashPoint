// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "System/FPGameInstance.h"

void ALobbyGameMode::StartMatch(EMatchMode MatchMode)
{
	// 로비에서 Seamless Travel을 수행하기 전, MatchInfo 데이터 캐싱
	if (UFPGameInstance* FPGameInstance = GetGameInstance<UFPGameInstance>())
	{
		FPGameInstance->CacheMatchInfo(MatchInfo);
	}
}
