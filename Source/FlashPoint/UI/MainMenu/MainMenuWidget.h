// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetInputInteraction.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class USizeBox;
class UMessagePopup;
class UWidgetSwitcher;
class UMatchListPage;
class UCreateMatchPopup;
class UButton;

/**
 * Main Menu를 표시하는 위젯
 */
UCLASS()
class FLASHPOINT_API UMainMenuWidget : public UUserWidget, public IWidgetInputInteraction
{
	GENERATED_BODY()

public:
	// Begin IWidgetInputInteraction
	virtual void Input_UI_Back() override;
	virtual void Input_UI_Confirm() override;
	// End IWidgetInputInteraction

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	// 로비 또는 매치에서 퇴장당해 메인메뉴로 이동한 경우, 그 이유를 띄운다.
	void TryShowKickReason() const;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> EmptyWidget;

	UFUNCTION()
	void HideWidgetSwitcher();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCreateMatchPopup> CreateMatchPopup;

	UFUNCTION()
	void OnCreateMatchPopupRoomNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

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
