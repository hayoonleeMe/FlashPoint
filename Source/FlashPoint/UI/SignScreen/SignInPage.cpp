// Fill out your copyright notice in the Description page of Project Settings.


#include "SignInPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Player/BasePlayerController.h"
#include "System/OnlineServiceSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SignInPage)

void USignInPage::InitializeWidget()
{
	TextBox_Username->SetText(FText::GetEmpty());
	TextBox_Password->SetText(FText::GetEmpty());
	Text_StatusMessage->SetText(FText::GetEmpty());

	UpdateSignInButtonState();
}

void USignInPage::Input_UI_Confirm()
{
	// Sign In 수행
	if (Button_SignIn->GetIsEnabled())
	{
		Button_SignIn->OnClicked.Broadcast();
	}
}

void USignInPage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (!OnlineServiceSubsystem->SignInStatusMessageDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->SignInStatusMessageDelegate.AddUObject(this, &ThisClass::UpdateStatusMessage);
		}
	}

	TextBox_Username->OnTextChanged.AddDynamic(this, &ThisClass::OnUsernameChanged);
	TextBox_Username->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextBoxCommitted);
	TextBox_Password->OnTextChanged.AddDynamic(this, &ThisClass::OnPasswordChanged);
	TextBox_Password->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextBoxCommitted);
}

void USignInPage::UpdateSignInButtonState()
{
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		const bool bCanEnableSignInButton = OnlineServiceSubsystem->ValidateSignInInput(TextBox_Username->GetText().ToString(), TextBox_Password->GetText().ToString());
		
		Button_SignIn->SetIsEnabled(bCanEnableSignInButton);
	}
}

void USignInPage::UpdateTextBox(const FText& Text, UEditableTextBox* TextBox)
{
	FString CurrentString = Text.ToString();
	CurrentString.ReplaceInline(TEXT(" "), TEXT(""));

	TextBox->SetText(FText::FromString(CurrentString));
	UpdateSignInButtonState();
}

void USignInPage::OnUsernameChanged(const FText& Text)
{
	UpdateTextBox(Text, TextBox_Username);
}

void USignInPage::OnTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (ABasePlayerController* BasePC = GetOwningPlayer<ABasePlayerController>())
		{
			// 제대로 입력이 들어갈 수 있도록 다시 설정
			BasePC->SetUIInputMode();
			Input_UI_Confirm();
		}
	}
}

void USignInPage::OnPasswordChanged(const FText& Text)
{
	UpdateTextBox(Text, TextBox_Password);
}

void USignInPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidget)
{
	Text_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidget)
	{
		Button_SignIn->SetIsEnabled(true);
	}
}
