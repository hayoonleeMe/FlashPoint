// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameState.h"

#include "Component/UIManageComponent.h"
#include "Player/FPPlayerState.h"
#include "UI/Gameplay/Scoreboard/Scoreboard.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameState)

void AFPGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (PlayerState)
	{
		if (AFPPlayerState* FPPlayerState = Cast<AFPPlayerState>(PlayerState))
		{
			FPPlayerState->OnPlayerKillCountChangedDelegate.AddUObject(this, &ThisClass::OnPlayerKillCountChanged);
		}
	}
}

void AFPGameState::RemovePlayerState(APlayerState* PlayerState)
{
	if (PlayerState)
	{
		if (AFPPlayerState* FPPlayerState = Cast<AFPPlayerState>(PlayerState))
		{
			FPPlayerState->OnPlayerKillCountChangedDelegate.RemoveAll(this);
		}
	}
	
	Super::RemovePlayerState(PlayerState);
}

void AFPGameState::ShowScoreboard(bool bShow) const
{
	if (Scoreboard)
	{
		Scoreboard->ShowScoreboard(bShow);
	}
}

int32 AFPGameState::GetTeamKillCount(ETeam Team) const
{
	return TeamKillCounts.FindRef(Team);
}

void AFPGameState::BeginPlay()
{
	Super::BeginPlay();

	OnClientMatchInfoReplicatedDelegate.AddUObject(this, &ThisClass::OnClientMatchInfoReplicated);
	OnClientPlayerInfoAddedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoAdded);
	OnClientPlayerInfoRemovedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoRemoved);
	OnClientPlayerInfoChangedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoChanged);
}

void AFPGameState::OnClientMatchInfoReplicated(const FMatchInfo& InMatchInfo)
{
	if (TSubclassOf<UScoreboard>* ScoreboardClass = ScoreboardClasses.Find(InMatchInfo.MatchMode))
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			if (UUIManageComponent* UIManageComponent = UUIManageComponent::Get(GameInstance->GetFirstLocalPlayerController()))
			{
				// Scoreboard Widget 생성 후 숨김
				Scoreboard = UIManageComponent->AddWidget<UScoreboard>(EWidgetLayer::HUD, *ScoreboardClass);
				ShowScoreboard(false);
			}
		}
	}
}

void AFPGameState::OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo)
{
	if (Scoreboard)
	{
		Scoreboard->OnPlayerAdded(PlayerInfo);
	}
}

void AFPGameState::OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo)
{
	if (Scoreboard)
	{
		Scoreboard->OnPlayerRemoved(PlayerInfo);
	}
}

void AFPGameState::OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo)
{
	if (Scoreboard)
	{
		Scoreboard->OnPlayerChanged(PlayerInfo);
	}
}

void AFPGameState::OnPlayerKillCountChanged(ETeam Team, int32 AddAmount)
{
	int32& TeamKillCount = TeamKillCounts.FindOrAdd(Team);
	TeamKillCount += AddAmount;
}
