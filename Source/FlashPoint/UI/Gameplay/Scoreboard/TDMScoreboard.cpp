// Fill out your copyright notice in the Description page of Project Settings.


#include "TDMScoreboard.h"

#include "ScoreboardRow.h"
#include "Components/VerticalBox.h"
#include "Data/PlayerInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TDMScoreboard)

void UTDMScoreboard::OnPlayerAdded(const FPlayerInfo& PlayerInfo)
{
	ETeam Team = PlayerInfo.Team;
	if (Team == ETeam::RedTeam)
	{
		RedTeamPlayers.Add(PlayerInfo);
		SortRowsByKillDeathIfVisible(VerticalBox_RedTeam, RedTeamPlayers);
	}
	else if (Team == ETeam::BlueTeam)
	{
		BlueTeamPlayers.Add(PlayerInfo);
		SortRowsByKillDeathIfVisible(VerticalBox_BlueTeam, BlueTeamPlayers);
	}
}

void UTDMScoreboard::OnPlayerRemoved(const FPlayerInfo& PlayerInfo)
{
	ETeam Team = PlayerInfo.Team;
	if (Team == ETeam::RedTeam)
	{
		// 할당된 메모리 유지
		RedTeamPlayers.RemoveSwap(PlayerInfo, EAllowShrinking::No);
		SortRowsByKillDeathIfVisible(VerticalBox_RedTeam, RedTeamPlayers);
	}
	else if (Team == ETeam::BlueTeam)
	{
		// 할당된 메모리 유지
		BlueTeamPlayers.RemoveSwap(PlayerInfo, EAllowShrinking::No);
		SortRowsByKillDeathIfVisible(VerticalBox_BlueTeam, BlueTeamPlayers);
	}
}

void UTDMScoreboard::OnPlayerChanged(const FPlayerInfo& PlayerInfo)
{
	ETeam Team = PlayerInfo.Team;
	if (Team == ETeam::RedTeam)
	{
		int32 Index;
		if (RedTeamPlayers.Find(PlayerInfo, Index))
		{
			RedTeamPlayers[Index] = PlayerInfo;
		}
		SortRowsByKillDeathIfVisible(VerticalBox_RedTeam, RedTeamPlayers);
	}
	else if (Team == ETeam::BlueTeam)
	{
		int32 Index;
		if (BlueTeamPlayers.Find(PlayerInfo, Index))
		{
			BlueTeamPlayers[Index] = PlayerInfo;
		}
		SortRowsByKillDeathIfVisible(VerticalBox_BlueTeam, BlueTeamPlayers);
	}
}

void UTDMScoreboard::ShowScoreboard(bool bShow)
{
	if (bShow)
	{
		SetVisibility(ESlateVisibility::Visible);
		SortRowsByKillDeathIfVisible(VerticalBox_RedTeam, RedTeamPlayers);
		SortRowsByKillDeathIfVisible(VerticalBox_BlueTeam, BlueTeamPlayers);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTDMScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RedTeamPlayers.Reserve(VerticalBox_RedTeam->GetChildrenCount());
	BlueTeamPlayers.Reserve(VerticalBox_BlueTeam->GetChildrenCount());
}

void UTDMScoreboard::SortRowsByKillDeathIfVisible(const UVerticalBox* BoxToSort, TArray<FPlayerInfo>& Players) const
{
	// 현재 화면에 표시 중이 아니면 수행하지 않음
	if (!IsVisible())
	{
		return;
	}
	
	// Sort by Kill, Death
	auto Predicate = [](const FPlayerInfo& A, const FPlayerInfo& B)
	{
		if (A.KillCount == B.KillCount)
		{
			return A.DeathCount < B.DeathCount;
		}
		return A.KillCount > B.KillCount;
	};
	Players.Sort(Predicate);

	const TArray<UWidget*>& Rows = BoxToSort->GetAllChildren();
	for (int32 Index = 0; Index < Rows.Num(); ++Index)
	{
		if (UScoreboardRow* Row = Cast<UScoreboardRow>(Rows[Index]))
		{
			if (Index < Players.Num())
			{
				const FPlayerInfo& PlayerInfo = Players[Index];
				Row->SetRow(PlayerInfo.Username, PlayerInfo.KillCount, PlayerInfo.DeathCount, PlayerInfo.Username == PlayerUsername);
			}
			else
			{
				Row->ShowRow(false);
			}
		}
	}
}
