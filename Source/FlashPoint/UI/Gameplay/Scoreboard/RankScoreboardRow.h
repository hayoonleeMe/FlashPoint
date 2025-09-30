// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreboardRow.h"
#include "RankScoreboardRow.generated.h"

/**
 * 기존 ScoreboardRow에 Rank를 함께 표시하는 위젯
 */
UCLASS()
class FLASHPOINT_API URankScoreboardRow : public UScoreboardRow
{
	GENERATED_BODY()

public:
	// 파라미터에 따라 해당 위젯을 설정하고 화면에 표시한다.
	void SetRow(int32 Rank, const FString& Username, int32 KillCount, int32 DeathCount, bool bIsLocalPlayer) const;

	virtual void ShowRow(bool bShow) const override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Rank;
};
