// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"

#include "FFAPlayerList.h"
#include "FPGameplayTags.h"
#include "TDMPlayerList.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Game/BaseGameState.h"
#include "Player/LobbyPlayerController.h"
#include "System/FPAssetManager.h"
#include "System/PlayerAuthSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LobbyWidget)

void ULobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ButtonBox_Host->SetVisibility(ESlateVisibility::Collapsed);
	ButtonBox_Client->SetVisibility(ESlateVisibility::Collapsed);

	Button_HostStart->OnClicked.AddDynamic(this, &ThisClass::OnHostStartButtonClicked);
	Button_HostQuit->OnClicked.AddDynamic(this, &ThisClass::ReturnToMainMenu);
	Button_ClientQuit->OnClicked.AddDynamic(this, &ThisClass::ReturnToMainMenu);

	if (GetWorld())
	{
		if (ABaseGameState* BaseGS = GetWorld()->GetGameState<ABaseGameState>())
		{
			BaseGS->OnClientMatchInfoReplicatedDelegate.AddUObject(this, &ThisClass::InitializeLobby);
			BaseGS->OnClientPlayerInfoAddedDelegate.AddUObject(this, &ThisClass::OnPlayerJoined);
			BaseGS->OnClientPlayerInfoRemovedDelegate.AddUObject(this, &ThisClass::OnPlayerLeft);
			BaseGS->OnClientPlayerInfoChangedDelegate.AddUObject(this, &ThisClass::OnPlayerUpdated);
		}
	}
}

void ULobbyWidget::InitializeLobby(const FMatchInfo& MatchInfo)
{
	MatchMode = MatchInfo.MatchMode;
	
	if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
	{
		PlayerUsername = PlayerAuthSubsystem->GetUsername();
	}
	
	Text_RoomName->SetText(FText::FromString(MatchInfo.RoomName));
	Text_MatchMode->SetText(FText::FromString(LexToString(MatchInfo.MatchMode)));

	if (MatchMode == EMatchMode::TeamDeathMatch)
	{
		WidgetSwitcher->SetActiveWidget(TDMPlayerList);
		TDMPlayerList->SetPlayerUsername(PlayerUsername);
	}
	else if (MatchMode == EMatchMode::FreeForAll)
	{
		WidgetSwitcher->SetActiveWidget(FFAPlayerList);
		FFAPlayerList->SetPlayerUsername(PlayerUsername);
	}

	const bool bIsHost = MatchInfo.HostId == PlayerUsername;
	ButtonBox_Host->SetVisibility(bIsHost ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ButtonBox_Client->SetVisibility(bIsHost ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
}

void ULobbyWidget::OnPlayerJoined(const FPlayerInfo& PlayerInfo) const
{
	if (MatchMode == EMatchMode::TeamDeathMatch)
	{
		TDMPlayerList->OnPlayerJoined(PlayerInfo.GetUsername(), PlayerInfo.Team);
	}
	else if (MatchMode == EMatchMode::FreeForAll)
	{
		FFAPlayerList->OnPlayerJoined(PlayerInfo.GetUsername());
	}
}

void ULobbyWidget::OnPlayerLeft(const FPlayerInfo& PlayerInfo) const
{
	if (MatchMode == EMatchMode::TeamDeathMatch)
	{
		TDMPlayerList->OnPlayerLeft(PlayerInfo.GetUsername());
	}
	else if (MatchMode == EMatchMode::FreeForAll)
	{
		FFAPlayerList->OnPlayerLeft(PlayerInfo.GetUsername());
	}
}

void ULobbyWidget::OnPlayerUpdated(const FPlayerInfo& PlayerInfo) const
{
	if (MatchMode == EMatchMode::TeamDeathMatch)
	{
		TDMPlayerList->OnPlayerUpdated(PlayerInfo.GetUsername(), PlayerInfo.Team);
	}
}

void ULobbyWidget::OnHostStartButtonClicked()
{
	if (ALobbyPlayerController* LobbyPC = GetOwningPlayer<ALobbyPlayerController>())
	{
		LobbyPC->ServerStartMatch(MatchMode);
	}
}

void ULobbyWidget::ReturnToMainMenu()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		const FString MainMenuLevelPath = UFPAssetManager::GetAssetPathByTag(FPGameplayTags::Asset::Level::MainMenu).GetAssetName();
		PC->ClientTravel(MainMenuLevelPath, TRAVEL_Absolute);
	}
}
