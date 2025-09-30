// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MatchTypes.h"
#include "LobbyWidget.generated.h"

struct FPlayerInfo;
class UButton;
class UHorizontalBox;
class UFFAPlayerList;
class UTDMPlayerList;
class UWidgetSwitcher;
class UTextBlock;

/**
 * Lobby Main Widget
 */
UCLASS()
class FLASHPOINT_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private:
	// MatchInfo로 위젯을 초기화한다.
	void InitializeLobby(const FMatchInfo& MatchInfo);
	
	void OnPlayerJoined(const FPlayerInfo& PlayerInfo) const;
	void OnPlayerLeft(const FPlayerInfo& PlayerInfo) const;
	void OnPlayerUpdated(const FPlayerInfo& PlayerInfo) const;
	
	// 로컬 플레이어의 Username
	FString PlayerUsername;

	// 현재 매치의 모드
	EMatchMode MatchMode;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_RoomName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_MatchMode;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_GoalKillCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTDMPlayerList> TDMPlayerList;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UFFAPlayerList> FFAPlayerList;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> ButtonBox_Host;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_HostStart;

	UFUNCTION()
	void OnHostStartButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_HostQuit;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> ButtonBox_Client;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_ClientQuit;

	UFUNCTION()
	void ReturnToMainMenu();
};
