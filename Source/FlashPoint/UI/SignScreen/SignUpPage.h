// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignUpPage.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;

/**
 * Sign Up 입력을 받고 수행하는 위젯
 */
UCLASS()
class FLASHPOINT_API USignUpPage : public UUserWidget
{
	GENERATED_BODY()

	friend class USignScreen;

public:
	void InitializeWidget();

protected:
	virtual void NativeOnInitialized() override;

private:
	void UpdateSignUpButtonState();

	void UpdateTextBox(const FText& Text, UEditableTextBox* TextBox);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Username;

	UFUNCTION()
	void OnUsernameChanged(const FText& Text);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Password;

	UFUNCTION()
	void OnPasswordChanged(const FText& Text);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_ConfirmPassword;

	UFUNCTION()
	void OnConfirmPasswordChanged(const FText& Text);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Email;

	UFUNCTION()
	void OnEmailChanged(const FText& Text);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SignUp;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Back;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StatusMessage;

	UFUNCTION()
	void UpdateStatusMessage(const FString& Message, bool bShouldResetWidget);
};
