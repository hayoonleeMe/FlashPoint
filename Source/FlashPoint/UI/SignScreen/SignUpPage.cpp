// Fill out your copyright notice in the Description page of Project Settings.


#include "SignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "System/OnlineServiceSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SignUpPage)

void USignUpPage::InitializeWidget()
{
	TextBox_Username->SetText(FText::GetEmpty());
	TextBox_Password->SetText(FText::GetEmpty());
	TextBox_ConfirmPassword->SetText(FText::GetEmpty());
	TextBox_Email->SetText(FText::GetEmpty());
	Text_StatusMessage->SetText(FText::GetEmpty());
	
	UpdateSignUpButtonState();
}

void USignUpPage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (!OnlineServiceSubsystem->SignUpStatusMessageDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->SignUpStatusMessageDelegate.AddUObject(this, &ThisClass::UpdateStatusMessage);
		}
	}
	
	TextBox_Username->OnTextChanged.AddDynamic(this, &USignUpPage::OnUsernameChanged);
	TextBox_Password->OnTextChanged.AddDynamic(this, &USignUpPage::OnPasswordChanged);
	TextBox_ConfirmPassword->OnTextChanged.AddDynamic(this, &USignUpPage::OnConfirmPasswordChanged);
	TextBox_Email->OnTextChanged.AddDynamic(this, &USignUpPage::OnEmailChanged);
}

void USignUpPage::UpdateSignUpButtonState()
{
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		const bool bCanEnableSignUpButton = OnlineServiceSubsystem->ValidateSignUpInput(TextBox_Username->GetText().ToString(), TextBox_Password->GetText().ToString(), TextBox_ConfirmPassword->GetText().ToString(), TextBox_Email->GetText().ToString());
		
		Button_SignUp->SetIsEnabled(bCanEnableSignUpButton);
	}
}

void USignUpPage::UpdateTextBox(const FText& Text, UEditableTextBox* TextBox)
{
	FString CurrentString = Text.ToString();
	CurrentString.ReplaceInline(TEXT(" "), TEXT(""));

	TextBox->SetText(FText::FromString(CurrentString));
	UpdateSignUpButtonState();
}

void USignUpPage::OnUsernameChanged(const FText& Text)
{
	UpdateTextBox(Text, TextBox_Username);
}

void USignUpPage::OnPasswordChanged(const FText& Text)
{
	UpdateTextBox(Text, TextBox_Password);
}

void USignUpPage::OnConfirmPasswordChanged(const FText& Text)
{
	UpdateTextBox(Text, TextBox_ConfirmPassword);
}

void USignUpPage::OnEmailChanged(const FText& Text)
{
	UpdateTextBox(Text, TextBox_Email);
}

void USignUpPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidget)
{
	Text_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidget)
	{
		Button_SignUp->SetIsEnabled(true);
		Button_Back->SetIsEnabled(true);
	}
}
