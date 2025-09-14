// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MatchTypes.h"
#include "TDMPlayerList.generated.h"

class ABasePlayerState;
class UPlayerListRow;
class UVerticalBox;
class UButton;

/**
 * Team Death Match 로비의 플레이어 리스트 위젯
 */
UCLASS()
class FLASHPOINT_API UTDMPlayerList : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerUsername(const FString& Username) { PlayerUsername = Username; }

	void OnPlayerJoined(const FString& Username, ETeam Team) const;
	void OnPlayerLeft(const FString& Username) const;
	void OnPlayerUpdated(const FString& Username, ETeam Team) const;

protected:
	virtual void NativeOnInitialized() override;

private:
	FString PlayerUsername;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerListRow> PlayerListRowClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_RedTeam;

	UFUNCTION()
	void OnRedTeamButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_BlueTeam;

	UFUNCTION()
	void OnBlueTeamButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_RedTeam;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_BlueTeam;
};
