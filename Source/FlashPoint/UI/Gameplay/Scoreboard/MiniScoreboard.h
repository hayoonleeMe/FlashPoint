// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MatchTypes.h"
#include "MiniScoreboard.generated.h"

class UTextBlock;

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

	// 위젯에 KillCount를 업데이트한다.
	virtual void UpdateKillCount(ETeam Team, int32 KillCount) { }

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_GoalKillCount;
};
