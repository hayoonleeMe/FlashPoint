// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameState.h"

#include "Net/UnrealNetwork.h"
#include "System/PlayerAuthSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseGameState)

void ABaseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseGameState, PlayerInfoArray);
	DOREPLIFETIME(ABaseGameState, MatchInfo);
}

void ABaseGameState::SetMatchInfo(const FMatchInfo& InMatchInfo)
{
	MatchInfo = InMatchInfo;
}

void ABaseGameState::AddPlayerInfo(const FString& Username, ETeam Team)
{
	PlayerInfoArray.AddPlayer(Username, Team);
}

void ABaseGameState::RemovePlayerInfo(const FString& Username)
{
	PlayerInfoArray.RemovePlayer(Username);
}

void ABaseGameState::UpdatePlayerInfo(const FString& Username, ETeam Team)
{
	PlayerInfoArray.UpdatePlayer(Username, Team);
}

void ABaseGameState::BeginPlay()
{
	Super::BeginPlay();

	if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
	{
		PlayerUsername = PlayerAuthSubsystem->GetUsername();
	}
}

void ABaseGameState::OnRep_MatchInfo()
{
	OnClientMatchInfoReplicatedDelegate.Broadcast(MatchInfo);
}

void ABaseGameState::OnRep_PlayerInfoArray()
{
	HandleAddedPlayerInfos(PlayerInfoArray.GetAddedPlayerInfos());
	HandleRemovedPlayerInfos(PlayerInfoArray.GetRemovedPlayerInfos());
	HandleChangedPlayerInfos(PlayerInfoArray.GetChangedPlayerInfos());
}

void ABaseGameState::HandleAddedPlayerInfos(TArray<FPlayerInfo>& AddedPlayerInfos) const
{
	if (!AddedPlayerInfos.IsEmpty())
	{
		for (const FPlayerInfo& AddedPlayerInfo : AddedPlayerInfos)
		{
			OnClientPlayerInfoAddedDelegate.Broadcast(AddedPlayerInfo.Username, AddedPlayerInfo.Team);
		}
		AddedPlayerInfos.Reset();
	}
}

void ABaseGameState::HandleRemovedPlayerInfos(TArray<FPlayerInfo>& RemovedPlayerInfos) const
{
	if (!RemovedPlayerInfos.IsEmpty())
	{
		for (const FPlayerInfo& RemovedPlayerInfo : RemovedPlayerInfos)
		{
			OnClientPlayerInfoRemovedDelegate.Broadcast(RemovedPlayerInfo.Username, ETeam::None);
		}
		RemovedPlayerInfos.Reset();
	}
}

void ABaseGameState::HandleChangedPlayerInfos(TArray<FPlayerInfo>& ChangedPlayerInfos) const
{
	if (!ChangedPlayerInfos.IsEmpty())
	{
		for (const FPlayerInfo& ChangedPlayerInfo : ChangedPlayerInfos)
		{
			OnClientPlayerInfoChangedDelegate.Broadcast(ChangedPlayerInfo.Username, ChangedPlayerInfo.Team);
		}
		ChangedPlayerInfos.Reset();
	}
}
