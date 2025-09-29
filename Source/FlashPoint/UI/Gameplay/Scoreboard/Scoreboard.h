// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Scoreboard.generated.h"

struct FPlayerInfo;

/**
 * Scoreboard Widget Abstract Base Class
 */
UCLASS(Abstract)
class FLASHPOINT_API UScoreboard : public UUserWidget
{
	GENERATED_BODY()

public:
	// PlayerInfo로 나타나는 플레이어가 추가될 때
	virtual void OnPlayerAdded(const FPlayerInfo& PlayerInfo) { }

	// PlayerInfo로 나타나는 플레이어가 제거될 때
	virtual void OnPlayerRemoved(const FPlayerInfo& PlayerInfo) { }

	// PlayerInfo로 나타나는 플레이어가 변경될 때
	virtual void OnPlayerChanged(const FPlayerInfo& PlayerInfo) { }

	// Scoreboard를 표시한다.
	virtual void ShowScoreboard(bool bShow) { }

protected:
	virtual void NativeOnInitialized() override;
	
	// 로컬 플레이어의 Username
	FString PlayerUsername;
};
