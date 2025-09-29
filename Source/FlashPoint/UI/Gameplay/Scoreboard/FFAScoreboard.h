// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Scoreboard.h"
#include "FFAScoreboard.generated.h"

class UVerticalBox;

/**
 * Free For All Scoreboard Widget
 */
UCLASS()
class FLASHPOINT_API UFFAScoreboard : public UScoreboard
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
	// 모든 Row Widget들을 PlayerInfos의 Kill, Death 정보를 토대로 정렬한다.
	// 현재 이 위젯이 화면에 표시 중일 때만 수행한다.
	void SortRowsByKillDeathIfVisible();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Right;

	// VerticalBox_Left와 VerticalBox_Right의 모든 Child Widget들을 캐싱하는 배열
	UPROPERTY()
	TArray<UWidget*> AllRowWidgets;

	// 위젯 관리를 위해 Player들에 대한 Info를 캐싱하는 배열
	TArray<FPlayerInfo> PlayerInfos;
};
