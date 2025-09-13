// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UMessagePopup;
class UWidgetSwitcher;
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

public:
	void ShowKickReason(const FString& KickReason) const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UFUNCTION()
	void HideWidgetSwitcher();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCreateMatchPopup> CreateMatchPopup;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMatchListPage> MatchListPage;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_CreateMatch;

	UFUNCTION()
	void OnCreateMatchButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_FindMatch;

	UFUNCTION()
	void OnFindMatchButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Settings;

	UFUNCTION()
	void OnSettingsButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Quit;

	UFUNCTION()
	void OnQuitButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMessagePopup> MessagePopup;

	UFUNCTION()
	void OnMessagePopupCloseButtonClicked();

	void ShowMessagePopup(bool bShow) const;

	// 로비에서 퇴장당했을 때 MessagePopup Title에 표시할 문자열
	static constexpr const TCHAR* DisconnectedText = TEXT("Disconnected");
};
