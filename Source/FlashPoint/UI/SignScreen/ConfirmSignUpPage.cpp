// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfirmSignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Player/BasePlayerController.h"
#include "System/OnlineServiceSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ConfirmSignUpPage)

void UConfirmSignUpPage::InitializeWidget()
{
	TextBox_ConfirmationCode->SetText(FText::GetEmpty());
	Text_StatusMessage->SetText(FText::GetEmpty());
	Text_Destination->SetText(FText::GetEmpty());

	UpdateConfirmButtonState();
}

void UConfirmSignUpPage::Input_UI_Confirm()
{
	// Confirm 수행
	if (Button_Confirm->GetIsEnabled())
	{
		Button_Confirm->OnClicked.Broadcast();
	}
}

void UConfirmSignUpPage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (!OnlineServiceSubsystem->ConfirmSignUpStatusMessageDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->ConfirmSignUpStatusMessageDelegate.AddUObject(this, &ThisClass::UpdateStatusMessage);
		}
	}
	
	TextBox_ConfirmationCode->OnTextChanged.AddDynamic(this, &ThisClass::UpdateConfirmButtonState);
	TextBox_ConfirmationCode->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextBoxCommitted);
}

void UConfirmSignUpPage::UpdateConfirmButtonState(const FText& Text)
{
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		const bool bValidConfirmationCode = OnlineServiceSubsystem->ValidateConfirmationCode(Text.ToString());
		
		Button_Confirm->SetIsEnabled(bValidConfirmationCode);
	}
}

void UConfirmSignUpPage::OnTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
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

void UConfirmSignUpPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidget)
{
	Text_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidget)
	{
		Button_Confirm->SetIsEnabled(true);
		Button_Back->SetIsEnabled(true);
	}
}
