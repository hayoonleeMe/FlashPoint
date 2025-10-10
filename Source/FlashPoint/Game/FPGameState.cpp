// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameState)

AFPGameState::AFPGameState()
{
	MatchEndTimeDilationDelay = 6.f;
	GlobalTimeDilation = 1.f;
}

void AFPGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPGameState, MatchEndTime);
	DOREPLIFETIME(AFPGameState, GlobalTimeDilation);
}

void AFPGameState::SetMatchEndTime(float InMatchEndTime)
{
	MatchEndTime = InMatchEndTime;
}

void AFPGameState::SetGlobalTimeDilation(float TimeDilation)
{
	GlobalTimeDilation = TimeDilation;
	OnRep_GlobalTimeDilation();
}

ETeam AFPGameState::GetWinningTeam() const
{
	if (MatchInfo.MatchMode == EMatchMode::FreeForAll)
	{
		// 개인전에 WinningTeam은 없으므로 ETeam::None 반환하고 종료
		return ETeam::None;
	}
	
	int32 MaxKillCount = -1;
	ETeam WinningTeam = ETeam::None;
	for (const auto& Pair : TeamKillCounts)
	{
		if (MaxKillCount < Pair.Value)
		{
			WinningTeam = Pair.Key;
			MaxKillCount = Pair.Value;
		}
		else if (MaxKillCount == Pair.Value)
		{
			// 모든 팀이 비기면 ETeam::None 반환
			WinningTeam = ETeam::None;
		}
	}
	return WinningTeam;
}

int32 AFPGameState::GetUserRank(const FString& Username)
{
	// Sort by Kill, Death
	auto Predicate = [](const FPlayerInfo& A, const FPlayerInfo& B)
	{
		if (A.KillCount == B.KillCount)
		{
			return A.DeathCount < B.DeathCount;
		}
		return A.KillCount > B.KillCount;
	};
	PlayerInfoArray.Sort(Predicate);

	const TArray<FPlayerInfo>& PlayerInfos = PlayerInfoArray.GetPlayers();
	for (int32 Index = 0; Index < PlayerInfos.Num(); ++Index)
	{
		if (PlayerInfos[Index].GetUsername() == Username)
		{
			return Index + 1;
		}
	}
	
	return 0;
}

void AFPGameState::BeginPlay()
{
	Super::BeginPlay();

	OnClientPlayerInfoAddedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoAdded);
	OnClientPlayerInfoRemovedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoRemoved);
	OnClientPlayerInfoChangedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoChanged);
}

void AFPGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	OnMatchEndedDelegate.Broadcast();
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

void AFPGameState::OnRep_GlobalTimeDilation()
{
	// 슬로우 모션
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GlobalTimeDilation);

	// Sound Pitch
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), GlobalTimeDilation, 0.6f);

	// MatchEndTimeDilationDelay 뒤 델레게이트 브로드캐스트
	// Time Dilation이 반영된 Rate로 설정
	FTimerHandle MatchTimer;
	GetWorldTimerManager().SetTimer(MatchTimer, FTimerDelegate::CreateUObject(this, &ThisClass::OnMatchEndTimeDilationFinished), MatchEndTimeDilationDelay * GlobalTimeDilation, false);
}

void AFPGameState::OnMatchEndTimeDilationFinished()
{
	if (HasAuthority())
	{
		GlobalTimeDilation = 1.f;
	}
	OnMatchEndTimeDilationFinishedDelegate.Broadcast();
}
