// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/WidgetInputInteraction.h"
#include "CreateMatchPopup.generated.h"

class UComboBoxString;
class UTextBlock;
class UButton;
class UEditableTextBox;

/**
 * Match를 생성하기 위한 입력을 받고 생성하는 팝업 위젯
 */
UCLASS()
class FLASHPOINT_API UCreateMatchPopup : public UUserWidget, public IWidgetInputInteraction
{
	GENERATED_BODY()

public:
	UCreateMatchPopup(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Begin IWidgetInputInteraction
	virtual void Input_UI_Back() override;
	virtual void Input_UI_Confirm() override;
	// End IWidgetInputInteraction

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void UpdateCreateButtonState(const FText& Text = FText());
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_RoomName;

	UFUNCTION()
	void OnRoomNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxString> ComboBox_MatchMode;

	UPROPERTY(EditDefaultsOnly)
	TArray<FString> MatchModeOptions;

	UFUNCTION()
	void OnMatchModeOptionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxString> ComboBox_MaxPlayers;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> TDMPlayerCountOptions;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> FFAPlayerCountOptions;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxString> ComboBox_GoalKillCount;

	UPROPERTY(EditDefaultsOnly)
	TArray<int32> GoalKillCountOptions;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Create;

	UFUNCTION()
	void OnCreateButtonClicked();

	void OnCreateMatchSucceeded(const FString& URL);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Cancel;

	UFUNCTION()
	void OnCancelButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StatusMessage;

	void UpdateStatusMessage(const FString& Message, bool bShouldResetWidget);
};
