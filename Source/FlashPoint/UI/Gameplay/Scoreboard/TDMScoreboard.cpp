// Fill out your copyright notice in the Description page of Project Settings.


#include "TDMScoreboard.h"

#include "ScoreboardRow.h"
#include "Components/VerticalBox.h"
#include "Data/PlayerInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TDMScoreboard)

void UTDMScoreboard::ShowWidget(bool bShow)
{
	if (bShow)
	{
		SetVisibility(ESlateVisibility::Visible);
		SortRowsByKillDeathIfVisible(RedTeamRowWidgets, RedTeamPlayers);
		SortRowsByKillDeathIfVisible(BlueTeamRowWidgets, BlueTeamPlayers);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTDMScoreboard::OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo)
{
	ETeam Team = PlayerInfo.Team;
	if (Team == ETeam::RedTeam)
	{
		RedTeamPlayers.Add(PlayerInfo);
		SortRowsByKillDeathIfVisible(RedTeamRowWidgets, RedTeamPlayers);
	}
	else if (Team == ETeam::BlueTeam)
	{
		BlueTeamPlayers.Add(PlayerInfo);
		SortRowsByKillDeathIfVisible(BlueTeamRowWidgets, BlueTeamPlayers);
	}
}

void UTDMScoreboard::OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo)
{
	ETeam Team = PlayerInfo.Team;
	if (Team == ETeam::RedTeam)
	{
		// 할당된 메모리 유지
		RedTeamPlayers.RemoveSwap(PlayerInfo, EAllowShrinking::No);
		SortRowsByKillDeathIfVisible(RedTeamRowWidgets, RedTeamPlayers);
	}
	else if (Team == ETeam::BlueTeam)
	{
		// 할당된 메모리 유지
		BlueTeamPlayers.RemoveSwap(PlayerInfo, EAllowShrinking::No);
		SortRowsByKillDeathIfVisible(BlueTeamRowWidgets, BlueTeamPlayers);
	}
}

void UTDMScoreboard::OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo)
{
	ETeam Team = PlayerInfo.Team;
	if (Team == ETeam::RedTeam)
	{
		int32 Index;
		if (RedTeamPlayers.Find(PlayerInfo, Index))
		{
			RedTeamPlayers[Index] = PlayerInfo;
			SortRowsByKillDeathIfVisible(RedTeamRowWidgets, RedTeamPlayers);
		}
	}
	else if (Team == ETeam::BlueTeam)
	{
		int32 Index;
		if (BlueTeamPlayers.Find(PlayerInfo, Index))
		{
			BlueTeamPlayers[Index] = PlayerInfo;
			SortRowsByKillDeathIfVisible(BlueTeamRowWidgets, BlueTeamPlayers);
		}
	}
}

void UTDMScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// GetAllChildren()는 모든 Child를 새로운 배열로 복사하기 때문에 캐싱해서 사용
	RedTeamRowWidgets = VerticalBox_RedTeam->GetAllChildren();
	RedTeamPlayers.Reserve(RedTeamRowWidgets.Num());
	
	BlueTeamRowWidgets = VerticalBox_BlueTeam->GetAllChildren();
	BlueTeamPlayers.Reserve(BlueTeamRowWidgets.Num());
}

void UTDMScoreboard::SortRowsByKillDeathIfVisible(const TArray<UWidget*>& RowWidgetsToSort, TArray<FPlayerInfo>& Players) const
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

	for (int32 Index = 0; Index < RowWidgetsToSort.Num(); ++Index)
	{
		if (UScoreboardRow* Row = Cast<UScoreboardRow>(RowWidgetsToSort[Index]))
		{
			if (Index < Players.Num())
			{
				const FPlayerInfo& PlayerInfo = Players[Index];
				Row->SetRow(PlayerInfo.Username, PlayerInfo.KillCount, PlayerInfo.DeathCount, PlayerInfo.Username == PlayerUsername);
			}
			else
			{
				// 현재 순서의 Row가 화면에서 숨겨져 있으면, 나머지는 이미 숨겨져 있으므로 더 이상 진행 X
				if (!Row->IsRowVisible())
				{
					break;
				}
				
				Row->ShowRow(false);
			}
		}
	}
}
