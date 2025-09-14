// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FFAPlayerList.generated.h"

class UPlayerListRow;
class UVerticalBox;

/**
 * Free For All 로비의 플레이어 리스트 위젯
 */
UCLASS()
class FLASHPOINT_API UFFAPlayerList : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerUsername(const FString& Username) { PlayerUsername = Username; }
	
	void OnPlayerJoined(const FString& Username) const;
	void OnPlayerLeft(const FString& Username) const;

private:
	FString PlayerUsername;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerListRow> PlayerListRowClass;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Right;
};
