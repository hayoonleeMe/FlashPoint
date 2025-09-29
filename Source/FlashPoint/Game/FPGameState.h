// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "FPGameState.generated.h"

class UScoreboard;

/**
 * 게임플레이에서 사용할 GameState
 */
UCLASS()
class FLASHPOINT_API AFPGameState : public ABaseGameState
{
	GENERATED_BODY()

public:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	void ShowScoreboard(bool bShow) const;
	int32 GetTeamKillCount(ETeam Team) const;

protected:
	virtual void BeginPlay() override;

private:
	// 클라이언트로 MatchInfo 프로퍼티가 Replicate될 때 호출되는 Callback
	void OnClientMatchInfoReplicated(const FMatchInfo& InMatchInfo);
	
	// 클라이언트에 PlayerInfo 추가가 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo);
	
	// 클라이언트에 PlayerInfo 제거가 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo);
	
	// 클라이언트에 PlayerInfo 변경이 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo);

	// Team 소속의 플레이어의 KillCount가 증가할 때 호출되는 Callback
	void OnPlayerKillCountChanged(ETeam Team, int32 AddAmount);
	
	// 팀 별 합산 KillCount
	// 클라이언트에서 업데이트된다.
	TMap<ETeam, int32> TeamKillCounts;

	// Cached Scoreboard Widget
	UPROPERTY()
	TObjectPtr<UScoreboard> Scoreboard;

	// MatchMode 별 Scoreboard Widget Class
	UPROPERTY(EditDefaultsOnly)
	TMap<EMatchMode, TSubclassOf<UScoreboard>> ScoreboardClasses;
};
