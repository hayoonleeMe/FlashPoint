// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scoreboard.h"
#include "TDMScoreboard.generated.h"

class UScoreboardRow;
class UVerticalBox;

/**
 * Team Death Match Scoreboard Widget
 */
UCLASS()
class FLASHPOINT_API UTDMScoreboard : public UScoreboard
{
	GENERATED_BODY()

public:
	// Begin UScoreboard
	virtual void OnPlayerAdded(const FPlayerInfo& PlayerInfo) override;
	virtual void OnPlayerRemoved(const FPlayerInfo& PlayerInfo) override;
	virtual void OnPlayerChanged(const FPlayerInfo& PlayerInfo) override;
	virtual void ShowScoreboard(bool bShow) override;
	// End UScoreboard

protected:
	virtual void NativeOnInitialized() override;

private:
	// BoxToSort 내부 UScoreboardRow 위젯을 Players의 Kill, Death 정보를 토대로 정렬한다.
	// 현재 이 위젯이 화면에 표시 중일 때만 수행한다.
	void SortRowsByKillDeathIfVisible(const UVerticalBox* BoxToSort, TArray<FPlayerInfo>& Players) const;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_RedTeam;

	// 위젯 관리를 위해 Red Team Player에 대한 Info를 캐싱하는 배열
	TArray<FPlayerInfo> RedTeamPlayers;
	
	UPROPERTY(meta=(BindWidget))
    TObjectPtr<UVerticalBox> VerticalBox_BlueTeam;

	// 위젯 관리를 위해 Blue Team Player에 대한 Info를 캐싱하는 배열
	TArray<FPlayerInfo> BlueTeamPlayers;
};
