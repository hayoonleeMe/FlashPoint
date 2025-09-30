// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniScoreboard.h"

#include "Game/BaseGameState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MiniScoreboard)

void UMiniScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (GetWorld())
	{
		if (ABaseGameState* BaseGS = GetWorld()->GetGameState<ABaseGameState>())
		{
			BaseGS->OnClientPlayerInfoAddedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoAdded);
			BaseGS->OnClientPlayerInfoRemovedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoRemoved);
			BaseGS->OnClientPlayerInfoChangedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoChanged);
		}
	}
}

void UMiniScoreboard::OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo)
{
	PlayerKillCounts.Add(PlayerInfo.GetUsernameAsFName(), PlayerInfo.KillCount);
}

void UMiniScoreboard::OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo)
{
	PlayerKillCounts.Remove(PlayerInfo.GetUsernameAsFName());
}

void UMiniScoreboard::OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo)
{
	int32& PrevKillCount = PlayerKillCounts.FindOrAdd(PlayerInfo.GetUsernameAsFName());
	if (PrevKillCount != PlayerInfo.KillCount)
	{
		// KillCount가 변경될 때, 위젯에 업데이트
		int32& CurrentKillCount = TeamKillCounts.FindOrAdd(PlayerInfo.Team);
		CurrentKillCount += PlayerInfo.KillCount - PrevKillCount;
		PrevKillCount = PlayerInfo.KillCount;
			
		UpdateKillCount(PlayerInfo.Team, CurrentKillCount);
	}
}
