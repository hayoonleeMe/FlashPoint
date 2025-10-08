// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameState.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameState)

void AFPGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPGameState, MatchEndTime);
}

void AFPGameState::SetMatchEndTime(float InMatchEndTime)
{
	MatchEndTime = InMatchEndTime;
}

void AFPGameState::BeginPlay()
{
	Super::BeginPlay();

	OnClientPlayerInfoAddedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoAdded);
	OnClientPlayerInfoRemovedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoRemoved);
	OnClientPlayerInfoChangedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoChanged);
}

void AFPGameState::OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo)
{
	PlayerKillCounts.Add(PlayerInfo.GetUsernameAsFName(), PlayerInfo.KillCount);
}

void AFPGameState::OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo)
{
	PlayerKillCounts.Remove(PlayerInfo.GetUsernameAsFName());
}

void AFPGameState::OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo)
{
	int32& PrevKillCount = PlayerKillCounts.FindOrAdd(PlayerInfo.GetUsernameAsFName());
	if (PrevKillCount != PlayerInfo.KillCount)
	{
		// KillCount가 변경될 때, 위젯에 업데이트
		int32& CurrentKillCount = TeamKillCounts.FindOrAdd(PlayerInfo.Team);
		CurrentKillCount += PlayerInfo.KillCount - PrevKillCount;
		PrevKillCount = PlayerInfo.KillCount;

		OnTeamKillCountUpdatedDelegate.Broadcast(PlayerInfo.Team, CurrentKillCount);
	}
}

void AFPGameState::OnRep_MatchEndTime()
{
	OnClientMatchEndTimeReplicatedDelegate.Broadcast(MatchEndTime);
}
