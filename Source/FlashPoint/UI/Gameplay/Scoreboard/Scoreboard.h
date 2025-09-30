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
	// Scoreboard를 표시한다.
	virtual void ShowWidget(bool bShow) { }

protected:
	virtual void NativeOnInitialized() override;
	
	// 로컬 플레이어의 Username
	FString PlayerUsername;
	
	// 클라이언트에 PlayerInfo 추가가 Replicate될 떄 호출되는 Callback
	virtual void OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo) { }
	
	// 클라이언트에 PlayerInfo 제거가 Replicate될 떄 호출되는 Callback
	virtual void OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo) { }
	
	// 클라이언트에 PlayerInfo 변경이 Replicate될 떄 호출되는 Callback
	virtual void OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo) { }
};
