// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MessagePopup.generated.h"

class USizeBox;
class FOnButtonClickedEvent;
class UButton;
class UTextBlock;

/**
 * Text와 Close Button으로 구성되는 간단한 팝업 위젯
 */
UCLASS()
class FLASHPOINT_API UMessagePopup : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTitleText(const FString& Str);
	void SetMessageText(const FString& Str);

	FOnButtonClickedEvent& OnCloseButtonClicked() const { return Button_Close->OnClicked; }

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Title;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Message;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Close;
};
