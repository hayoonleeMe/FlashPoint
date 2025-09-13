// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateMatchPopup.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Data/MatchTypes.h"
#include "System/OnlineServiceSubsystem.h"
#include "System/PlayerAuthSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CreateMatchPopup)

UCreateMatchPopup::UCreateMatchPopup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MatchModeOptions = { LexToString(EMatchMode::TeamDeathMatch), LexToString(EMatchMode::FreeForAll) };
	
	TDMPlayerCountOptions = { 2, 4, 6, 8, 10 };
	FFAPlayerCountOptions = { 2, 3, 4, 5, 6, 7, 8, 9, 10 };
}

void UCreateMatchPopup::InitializeWidget()
{
	TextBox_RoomName->SetText(FText::GetEmpty());
	Text_StatusMessage->SetText(FText::GetEmpty());

	ComboBox_MatchMode->ClearOptions();
	for (const FString& Option : MatchModeOptions)
	{
		ComboBox_MatchMode->AddOption(Option);
	}
	ComboBox_MatchMode->SetSelectedIndex(0);

	ComboBox_MaxPlayers->ClearOptions();
	for (int32 Option : TDMPlayerCountOptions)
	{
		ComboBox_MaxPlayers->AddOption(FString::FromInt(Option));
	}
	ComboBox_MaxPlayers->SetSelectedIndex(0);

	UpdateCreateButtonState();

	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (!OnlineServiceSubsystem->CreateGameSessionStatusMessageDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->CreateGameSessionStatusMessageDelegate.AddUObject(this, &ThisClass::UpdateStatusMessage);	
		}
	}
}

void UCreateMatchPopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ComboBox_MatchMode->OnSelectionChanged.AddDynamic(this, &ThisClass::OnMatchModeOptionChanged);
	TextBox_RoomName->OnTextChanged.AddDynamic(this, &ThisClass::UpdateCreateButtonState);
	Button_Create->OnClicked.AddDynamic(this, &ThisClass::OnCreateButtonClicked);
}

void UCreateMatchPopup::UpdateCreateButtonState(const FText& Text)
{
	const bool bCanEnableCreateButton = !TextBox_RoomName->GetText().IsEmpty();
	Button_Create->SetIsEnabled(bCanEnableCreateButton);

	Text_StatusMessage->SetText(FText::FromString(bCanEnableCreateButton ? TEXT("") : OnlineServiceStatusMessage::CreateGameSession::InvalidRoomName));
}

void UCreateMatchPopup::OnMatchModeOptionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StringToLex(SelectedItem) == EMatchMode::TeamDeathMatch)
	{
		ComboBox_MaxPlayers->ClearOptions();
		for (int32 Option : TDMPlayerCountOptions)
		{
			ComboBox_MaxPlayers->AddOption(FString::FromInt(Option));
		}
		ComboBox_MaxPlayers->SetSelectedIndex(0);
	}
	else if (StringToLex(SelectedItem) == EMatchMode::FreeForAll)
	{
		ComboBox_MaxPlayers->ClearOptions();
		for (int32 Option : FFAPlayerCountOptions)
		{
			ComboBox_MaxPlayers->AddOption(FString::FromInt(Option));
		}
		ComboBox_MaxPlayers->SetSelectedIndex(0);
	}
}

void UCreateMatchPopup::OnCreateButtonClicked()
{
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
		{
			if (!OnlineServiceSubsystem->OnCreatePlayerSessionSucceededDelegate.IsBoundToObject(this))
			{
				OnlineServiceSubsystem->OnCreatePlayerSessionSucceededDelegate.AddUObject(this, &ThisClass::OnCreateMatchSucceeded);
			}
			
			const FString RoomName = TextBox_RoomName->GetText().ToString();
			const FString MatchMode = ComboBox_MatchMode->GetSelectedOption();
			const FString MaxPlayers = ComboBox_MaxPlayers->GetSelectedOption();
			const FString Username = PlayerAuthSubsystem->GetUsername();	// 방장 이름
			OnlineServiceSubsystem->CreateGameSession(RoomName, MatchMode, MaxPlayers, Username);
		}

		Button_Create->SetIsEnabled(false);
		Button_Cancel->SetIsEnabled(false);
	}
}

void UCreateMatchPopup::OnCreateMatchSucceeded(const FString& URL)
{
	// 생성된 매치의 서버 주소로 접속
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->ClientTravel(URL, TRAVEL_Absolute);
	}
}

void UCreateMatchPopup::UpdateStatusMessage(const FString& Message, bool bShouldResetWidget)
{
	Text_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidget)
	{
		UpdateCreateButtonState();
		Button_Cancel->SetIsEnabled(true);
	}
}
