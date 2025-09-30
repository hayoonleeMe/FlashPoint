// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MatchTypes.h"
#include "MiniScoreboard.generated.h"

class UTextBlock;
struct FPlayerInfo;

/**
 * Mini Scoreboard Widget Abstract Base Class
 */
UCLASS(Abstract)
class FLASHPOINT_API UMiniScoreboard : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetGoalKillCount(int32 GoalKillCount);

protected:
	virtual void NativeOnInitialized() override;

	// 클라이언트에 PlayerInfo 추가가 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo);
	
	// 클라이언트에 PlayerInfo 제거가 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo);

	// 클라이언트에 PlayerInfo 변경이 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo);

	// 위젯에 KillCount를 업데이트한다.
	virtual void UpdateKillCount(ETeam Team, int32 KillCount) { }

	// 팀 별 합산 KillCount
	TMap<ETeam, int32> TeamKillCounts;

	// 플레이어 별 KillCount
	// Key: Username, Value: KillCount
	TMap<FName, int32> PlayerKillCounts;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_GoalKillCount;
};
