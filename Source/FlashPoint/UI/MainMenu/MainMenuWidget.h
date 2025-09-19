// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UMessagePopup;
class UMatchListPage;
class UCreateMatchPopup;
class UButton;

/**
 * Main Menu를 표시하는 위젯
 */
UCLASS()
class FLASHPOINT_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

private:
	// 로비 또는 매치에서 퇴장당해 메인메뉴로 이동한 경우, 그 이유를 띄운다.
	void TryShowKickReason() const;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_CreateMatch;

	UFUNCTION()
	void OnCreateMatchButtonClicked();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCreateMatchPopup> CreateMatchPopupClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_FindMatch;

	UFUNCTION()
	void OnFindMatchButtonClicked();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMatchListPage> MatchListPageClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Settings;

	UFUNCTION()
	void OnSettingsButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Quit;

	UFUNCTION()
	void OnQuitButtonClicked();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMessagePopup> MessagePopupClass;
	
	// 로비에서 퇴장당했을 때 MessagePopup Title에 표시할 문자열
	static constexpr const TCHAR* DisconnectedText = TEXT("Disconnected");
};
