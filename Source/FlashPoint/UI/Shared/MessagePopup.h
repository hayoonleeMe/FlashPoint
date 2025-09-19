// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "UI/WidgetInputInteraction.h"
#include "MessagePopup.generated.h"

class UButton;
class UTextBlock;

/**
 * Text와 Close Button으로 구성되는 간단한 팝업 위젯
 */
UCLASS()
class FLASHPOINT_API UMessagePopup : public UUserWidget, public IWidgetInputInteraction
{
	GENERATED_BODY()
	
public:
	// Begin IWidgetInputInteraction
	virtual void Input_UI_Back() override;
	// End IWidgetInputInteraction
	
	void SetTitleText(const FString& Str);
	void SetMessageText(const FString& Str);

protected:
	virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Message;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Close;

	UFUNCTION()
	void OnCloseButtonClicked();
};
