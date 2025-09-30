// Fill out your copyright notice in the Description page of Project Settings.


#include "FFAScoreboard.h"

#include "RankScoreboardRow.h"
#include "Components/VerticalBox.h"
#include "Data/PlayerInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FFAScoreboard)

void UFFAScoreboard::ShowWidget(bool bShow)
{
	if (bShow)
	{
		SetVisibility(ESlateVisibility::Visible);
		SortRowsByKillDeathIfVisible();
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UFFAScoreboard::OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo)
{
	PlayerInfos.Add(PlayerInfo);
	SortRowsByKillDeathIfVisible();
}

void UFFAScoreboard::OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo)
{
	// 할당된 메모리 유지
	PlayerInfos.RemoveSwap(PlayerInfo, EAllowShrinking::No);
	SortRowsByKillDeathIfVisible();
}

void UFFAScoreboard::OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo)
{
	int32 Index;
	if (PlayerInfos.Find(PlayerInfo, Index))
	{
		PlayerInfos[Index] = PlayerInfo;
		SortRowsByKillDeathIfVisible();
	}
}

void UFFAScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// GetAllChildren()는 모든 Child를 새로운 배열로 복사하기 때문에 캐싱해서 사용
	AllRowWidgets.Reserve(VerticalBox_Left->GetChildrenCount() + VerticalBox_Right->GetChildrenCount());
	for (UWidget* Child : VerticalBox_Left->GetAllChildren())
	{
		AllRowWidgets.Add(Child);
	}
	for (UWidget* Child : VerticalBox_Right->GetAllChildren())
	{
		AllRowWidgets.Add(Child);
	}
	
	PlayerInfos.Reserve(AllRowWidgets.Num());
}

void UFFAScoreboard::SortRowsByKillDeathIfVisible()
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
	PlayerInfos.Sort(Predicate);

	for (int32 Index = 0; Index < AllRowWidgets.Num(); ++Index)
	{
		if (URankScoreboardRow* Row = Cast<URankScoreboardRow>(AllRowWidgets[Index]))
		{
			if (Index < PlayerInfos.Num())
			{
				const FPlayerInfo& PlayerInfo = PlayerInfos[Index];
				Row->SetRow(Index + 1, PlayerInfo.Username, PlayerInfo.KillCount, PlayerInfo.DeathCount, PlayerInfo.Username == PlayerUsername);
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
