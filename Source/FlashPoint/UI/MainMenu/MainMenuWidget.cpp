// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CreateMatchPopup.h"
#include "MatchListPage.h"
#include "Component/UIManageComponent.h"
#include "Game/MainMenuGameMode.h"
#include "UI/Shared/MessagePopup.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MainMenuWidget)

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_CreateMatch->OnClicked.AddDynamic(this, &ThisClass::OnCreateMatchButtonClicked);
	Button_FindMatch->OnClicked.AddDynamic(this, &ThisClass::OnFindMatchButtonClicked);
	Button_Settings->OnClicked.AddDynamic(this, &ThisClass::OnSettingsButtonClicked);
	Button_Quit->OnClicked.AddDynamic(this, &ThisClass::OnQuitButtonClicked);
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
				if (UUIManageComponent* UIManageComponent = UUIManageComponent::Get(GetOwningPlayer()))
				{
					if (UMessagePopup* MessagePopup = UIManageComponent->AddWidget<UMessagePopup>(EWidgetLayer::Popup, MessagePopupClass))
					{
						MessagePopup->SetTitleText(DisconnectedText);
						MessagePopup->SetMessageText(KickReason);
					}
				}
			}
		}
	}
}

void UMainMenuWidget::OnCreateMatchButtonClicked()
{
	if (UUIManageComponent* UIManageComponent = UUIManageComponent::Get(GetOwningPlayer()))
	{
		UIManageComponent->AddWidget(EWidgetLayer::Popup, CreateMatchPopupClass);
	}
}

void UMainMenuWidget::OnFindMatchButtonClicked()
{
	if (UUIManageComponent* UIManageComponent = UUIManageComponent::Get(GetOwningPlayer()))
	{
		UIManageComponent->AddWidget(EWidgetLayer::Menu, MatchListPageClass);
	}
}

void UMainMenuWidget::OnSettingsButtonClicked()
{
	// TODO
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
