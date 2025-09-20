// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/WidgetInputInteraction.h"
#include "PauseMenu.generated.h"

class UButton;

/**
 * 매치 내에서 Escape 키 입력 시 표시할 Pause Menu Widget
 */
UCLASS()
class FLASHPOINT_API UPauseMenu : public UUserWidget, public IWidgetInputInteraction
{
	GENERATED_BODY()

public:
	// Begin IWidgetInputInteraction
	virtual void Input_UI_Back() override;
	// End IWidgetInputInteraction

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Resume;

	UFUNCTION()
	void OnResumeButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Settings;

	UFUNCTION()
	void OnSettingsButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Quit;

	UFUNCTION()
	void ReturnToMainMenu();
};
