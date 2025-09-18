// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/WidgetInputInteraction.h"
#include "SignInPage.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;

/**
 * Sign In 입력을 받고 수행하는 위젯
 */
UCLASS()
class FLASHPOINT_API USignInPage : public UUserWidget, public IWidgetInputInteraction
{
	GENERATED_BODY()

	friend class USignScreen;

public:
	void InitializeWidget();

	// Begin IWidgetInputInteraction
	virtual void Input_UI_Confirm() override;
	// End IWidgetInputInteraction
	
protected:
	virtual void NativeOnInitialized() override;

private:
	void UpdateSignInButtonState();
	
	void UpdateTextBox(const FText& Text, UEditableTextBox* TextBox);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Username;

	UFUNCTION()
	void OnUsernameChanged(const FText& Text);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> TextBox_Password;

	UFUNCTION()
	void OnTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnPasswordChanged(const FText& Text);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SignIn;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SignUp;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Quit;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StatusMessage;

	void UpdateStatusMessage(const FString& Message, bool bShouldResetWidget);
};
