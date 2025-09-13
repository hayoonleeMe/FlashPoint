// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfirmSignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "System/OnlineServiceSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ConfirmSignUpPage)

void UConfirmSignUpPage::InitializeWidget()
{
	TextBox_ConfirmationCode->SetText(FText::GetEmpty());
	Text_StatusMessage->SetText(FText::GetEmpty());
	Text_Destination->SetText(FText::GetEmpty());

	UpdateConfirmButtonState();
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
}

void UConfirmSignUpPage::UpdateConfirmButtonState(const FText& Text)
{
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		const bool bValidConfirmationCode = OnlineServiceSubsystem->ValidateConfirmationCode(Text.ToString());
		
		Button_Confirm->SetIsEnabled(bValidConfirmationCode);
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
