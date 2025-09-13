// Fill out your copyright notice in the Description page of Project Settings.


#include "SignScreen.h"

#include "ConfirmSignUpPage.h"
#include "FPGameplayTags.h"
#include "SignInPage.h"
#include "SignUpPage.h"
#include "SuccessConfirmedPage.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "System/FPAssetManager.h"
#include "System/OnlineServiceSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SignScreen)

void USignScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this);
	check(OnlineServiceSubsystem);

	SignInPage->Button_SignIn->OnClicked.AddDynamic(this, &ThisClass::OnSignInButtonClicked);
	SignInPage->Button_SignUp->OnClicked.AddDynamic(this, &ThisClass::ShowSignUpPage);
	SignInPage->Button_Quit->OnClicked.AddDynamic(this, &ThisClass::OnQuitButtonClicked);
	if (!OnlineServiceSubsystem->OnSignInSucceededDelegate.IsBoundToObject(this))
	{
		OnlineServiceSubsystem->OnSignInSucceededDelegate.AddUObject(this, &ThisClass::OnSignInSucceeded);
	}

	SignUpPage->Button_Back->OnClicked.AddDynamic(this, &ThisClass::ShowSignInPage);
	SignUpPage->Button_SignUp->OnClicked.AddDynamic(this, &ThisClass::OnSignUpButtonClicked);
	if (!OnlineServiceSubsystem->OnSignUpSucceededDelegate.IsBoundToObject(this))
	{
		OnlineServiceSubsystem->OnSignUpSucceededDelegate.AddUObject(this, &ThisClass::OnSignUpSucceeded);
	}

	ConfirmSignUpPage->Button_Confirm->OnClicked.AddDynamic(this, &ThisClass::OnConfirmButtonClicked);
	ConfirmSignUpPage->Button_Back->OnClicked.AddDynamic(this, &ThisClass::ShowSignUpPage);
	if (!OnlineServiceSubsystem->OnConfirmSignUpSucceededDelegate.IsBoundToObject(this))
	{
		OnlineServiceSubsystem->OnConfirmSignUpSucceededDelegate.AddUObject(this, &ThisClass::OnConfirmSignUpSucceeded);
	}

	SuccessConfirmedPage->Button_Ok->OnClicked.AddDynamic(this, &ThisClass::ShowSignInPage);

	ShowSignInPage();
}

void USignScreen::ShowSignInPage()
{
	WidgetSwitcher->SetActiveWidget(SignInPage);
	SignInPage->InitializeWidget();
}

void USignScreen::ShowSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(SignUpPage);
	SignUpPage->InitializeWidget();
}

void USignScreen::ShowConfirmSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(ConfirmSignUpPage);
	ConfirmSignUpPage->InitializeWidget();
}

void USignScreen::ShowSuccessConfirmedPage()
{
	WidgetSwitcher->SetActiveWidget(SuccessConfirmedPage);
}

void USignScreen::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void USignScreen::OnSignInButtonClicked()
{
	SignInPage->Button_SignIn->SetIsEnabled(false);
	const FString Username = SignInPage->TextBox_Username->GetText().ToString();
	const FString Password = SignInPage->TextBox_Password->GetText().ToString();
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		OnlineServiceSubsystem->SignIn(Username, Password);
	}
}

void USignScreen::OnSignUpButtonClicked()
{
	SignUpPage->Button_SignUp->SetIsEnabled(false);
	SignUpPage->Button_Back->SetIsEnabled(false);
	
	const FString Username = SignUpPage->TextBox_Username->GetText().ToString();
	const FString Password = SignUpPage->TextBox_Password->GetText().ToString();
	const FString Email = SignUpPage->TextBox_Email->GetText().ToString();
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		OnlineServiceSubsystem->SignUp(Username, Password, Email);
	}
}

void USignScreen::OnConfirmButtonClicked()
{
	ConfirmSignUpPage->Button_Confirm->SetIsEnabled(false);
	ConfirmSignUpPage->Button_Back->SetIsEnabled(false);
	
	const FString ConfirmationCode = ConfirmSignUpPage->TextBox_ConfirmationCode->GetText().ToString();
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		OnlineServiceSubsystem->ConfirmSignUp(ConfirmationCode);
	}
}

void USignScreen::OnSignInSucceeded()
{
	const FName MainMenuLevelPath = UFPAssetManager::GetAssetPathByTag(FPGameplayTags::Asset::Level::MainMenu).GetAssetFName();
	UGameplayStatics::OpenLevel(this, MainMenuLevelPath);
}

void USignScreen::OnSignUpSucceeded(const FString& CodeDeliveryDestination)
{
	ConfirmSignUpPage->Text_Destination->SetText(FText::FromString(CodeDeliveryDestination));
	ShowConfirmSignUpPage();
}

void USignScreen::OnConfirmSignUpSucceeded()
{
	ShowSuccessConfirmedPage();
}
