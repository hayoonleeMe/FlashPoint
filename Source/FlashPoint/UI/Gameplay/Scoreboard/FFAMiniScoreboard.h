// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MiniScoreboard.h"
#include "FFAMiniScoreboard.generated.h"

class UTextBlock;

/**
 * Free For All Mini Scoreboard Widget
 */
UCLASS()
class FLASHPOINT_API UFFAMiniScoreboard : public UMiniScoreboard
{
	GENERATED_BODY()

protected:
	// Begin UMiniScoreboard
	virtual void UpdateKillCount(ETeam Team, int32 KillCount) override;
	// End UMiniScoreboard

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_KillCount;

	// TODO : Goal Kill Count
};
