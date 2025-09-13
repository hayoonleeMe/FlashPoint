// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmSignUpPage.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;

/**
 * Sign Up을 위한 Confirmation Code를 입력받고 수행하는 위젯
 */
UCLASS()
class FLASHPOINT_API UConfirmSignUpPage : public UUserWidget
{
	GENERATED_BODY()

	friend class USignScreen;
	
public:
	void InitializeWidget();
	
protected:
	virtual void NativeOnInitialized() override;
	
private:
	UFUNCTION()
	void UpdateConfirmButtonState(const FText& Text = FText());
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_ConfirmationCode;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Confirm;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Destination;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Back;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StatusMessage;

	UFUNCTION()
	void UpdateStatusMessage(const FString& Message, bool bShouldResetWidget);
};
