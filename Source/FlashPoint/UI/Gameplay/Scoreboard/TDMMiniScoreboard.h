// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MiniScoreboard.h"
#include "TDMMiniScoreboard.generated.h"

class UTextBlock;

/**
 * Team Death Match Mini Scoreboard Widget
 */
UCLASS()
class FLASHPOINT_API UTDMMiniScoreboard : public UMiniScoreboard
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	
	// Begin UMiniScoreboard
	virtual void UpdateKillCount(ETeam Team, int32 KillCount) override;
	// End UMiniScoreboard

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_RedKillCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_BlueKillCount;

	// TODO : Goal Kill Count
};
