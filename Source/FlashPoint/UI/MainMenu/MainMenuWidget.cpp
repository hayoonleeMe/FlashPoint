// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CreateMatchPopup.h"
#include "MatchListPage.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Components/WidgetSwitcher.h"
#include "Game/MainMenuGameMode.h"
#include "Player/BasePlayerController.h"
#include "UI/Shared/MessagePopup.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MainMenuWidget)

void UMainMenuWidget::Input_UI_Back()
{
	if (MessagePopup->GetVisibility() == ESlateVisibility::Visible)
	{
		// MessagePopup 표시중이면 닫기
		OnMessagePopupCloseButtonClicked();
	}
	else if (WidgetSwitcher->GetActiveWidget() == CreateMatchPopup)
	{
		// CreateMatchPopup에서 Cancel 할 수 있을 때 닫기
		if (CreateMatchPopup->Button_Cancel->GetIsEnabled())
		{
			HideWidgetSwitcher();
		}
	}
	else
	{
		HideWidgetSwitcher();
	}
}

void UMainMenuWidget::Input_UI_Confirm()
{
	if (IWidgetInputInteraction* WidgetInputInteraction = Cast<IWidgetInputInteraction>(WidgetSwitcher->GetActiveWidget()))
	{
		WidgetInputInteraction->Input_UI_Confirm();
	}
}

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CreateMatchPopup->Button_Cancel->OnClicked.AddDynamic(this, &ThisClass::HideWidgetSwitcher);
	CreateMatchPopup->TextBox_RoomName->OnTextCommitted.AddDynamic(this, &ThisClass::OnCreateMatchPopupRoomNameTextCommitted);
	MatchListPage->Button_Back->OnClicked.AddDynamic(this, &ThisClass::HideWidgetSwitcher);
	
	Button_CreateMatch->OnClicked.AddDynamic(this, &ThisClass::OnCreateMatchButtonClicked);
	Button_FindMatch->OnClicked.AddDynamic(this, &ThisClass::OnFindMatchButtonClicked);
	Button_Settings->OnClicked.AddDynamic(this, &ThisClass::OnSettingsButtonClicked);
	Button_Quit->OnClicked.AddDynamic(this, &ThisClass::OnQuitButtonClicked);

	MessagePopup->OnCloseButtonClicked().AddDynamic(this, &ThisClass::OnMessagePopupCloseButtonClicked);
}

void UMainMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	HideWidgetSwitcher();
	ShowMessagePopup(false);
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TryShowKickReason();
}

void UMainMenuWidget::TryShowKickReason() const
{
	if (GetWorld())
	{
		if (AMainMenuGameMode* MainMenuGameMode = GetWorld()->GetAuthGameMode<AMainMenuGameMode>())
		{
			const FString KickReason = MainMenuGameMode->GetParsedKickReasonOption();
			if (!KickReason.IsEmpty())
			{
				MessagePopup->SetTitleText(DisconnectedText);
				MessagePopup->SetMessageText(KickReason);
				ShowMessagePopup(true);
			}
		}
	}
}

void UMainMenuWidget::HideWidgetSwitcher()
{
	WidgetSwitcher->SetActiveWidget(EmptyWidget);
}

void UMainMenuWidget::OnCreateMatchPopupRoomNameTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (ABasePlayerController* BasePC = GetOwningPlayer<ABasePlayerController>())
		{
			// 제대로 입력이 들어갈 수 있도록 다시 설정
			BasePC->SetInitialInputMode();
			Input_UI_Confirm();
		}
	}
	else if (CommitMethod == ETextCommit::OnCleared)
	{
		if (ABasePlayerController* BasePC = GetOwningPlayer<ABasePlayerController>())
		{
			// 제대로 입력이 들어갈 수 있도록 다시 설정
			BasePC->SetInitialInputMode();
			Input_UI_Back();			
		}
	}
}

void UMainMenuWidget::OnCreateMatchButtonClicked()
{
	WidgetSwitcher->SetActiveWidget(CreateMatchPopup);
	CreateMatchPopup->InitializeWidget();
}

void UMainMenuWidget::OnFindMatchButtonClicked()
{
	WidgetSwitcher->SetActiveWidget(MatchListPage);
	MatchListPage->InitializeWidget();
}

void UMainMenuWidget::OnSettingsButtonClicked()
{
	// TODO
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UMainMenuWidget::OnMessagePopupCloseButtonClicked()
{
	ShowMessagePopup(false);
}

void UMainMenuWidget::ShowMessagePopup(bool bShow) const
{
	MessagePopup->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
