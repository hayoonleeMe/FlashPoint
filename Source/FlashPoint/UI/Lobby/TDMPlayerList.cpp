// Fill out your copyright notice in the Description page of Project Settings.


#include "TDMPlayerList.h"

#include "PlayerListRow.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Data/MatchTypes.h"
#include "Player/BasePlayerState.h"
#include "Player/LobbyPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TDMPlayerList)

void UTDMPlayerList::OnPlayerJoined(const FString& Username, ETeam Team) const
{
	if (Team != ETeam::None)
	{
		UVerticalBox* BoxToAdd = Team == ETeam::RedTeam ? VerticalBox_RedTeam : VerticalBox_BlueTeam;
		if (UPlayerListRow* PlayerListRow = CreateWidget<UPlayerListRow>(BoxToAdd, PlayerListRowClass))
		{
			PlayerListRow->SetUsername(Username);
			if (Username == PlayerUsername)
			{
				// 플레이어 이름 강조 표시
				PlayerListRow->HighlightUsername();
			}

			BoxToAdd->AddChildToVerticalBox(PlayerListRow);
		}
	}
}

void UTDMPlayerList::OnPlayerLeft(const FString& Username) const
{
	// RedBox에서 찾아서 제거
	for (UWidget* Widget : VerticalBox_RedTeam->GetAllChildren())
	{
		if (UPlayerListRow* PlayerListRow = Cast<UPlayerListRow>(Widget))
		{
			if (PlayerListRow->GetUsername() == Username)
			{
				VerticalBox_RedTeam->RemoveChild(PlayerListRow);
				return;
			}
		}
	}

	// BlueBox에서 찾아서 제거
	for (UWidget* Widget : VerticalBox_BlueTeam->GetAllChildren())
	{
		if (UPlayerListRow* PlayerListRow = Cast<UPlayerListRow>(Widget))
		{
			if (PlayerListRow->GetUsername() == Username)
			{
				VerticalBox_BlueTeam->RemoveChild(PlayerListRow);
				return;
			}
		}
	}
}

void UTDMPlayerList::OnPlayerUpdated(const FString& Username, ETeam Team) const
{
	if (Team == ETeam::RedTeam)
	{
		// Blue -> Red
		for (UWidget* Widget : VerticalBox_BlueTeam->GetAllChildren())
		{
			if (UPlayerListRow* PlayerListRow = Cast<UPlayerListRow>(Widget))
			{
				if (PlayerListRow->GetUsername() == Username)
				{
					VerticalBox_BlueTeam->RemoveChild(PlayerListRow);

					VerticalBox_RedTeam->AddChildToVerticalBox(PlayerListRow);
				}
			}
		}
	}
	else if (Team == ETeam::BlueTeam)
	{
		// Red -> Blue
		for (UWidget* Widget : VerticalBox_RedTeam->GetAllChildren())
		{
			if (UPlayerListRow* PlayerListRow = Cast<UPlayerListRow>(Widget))
			{
				if (PlayerListRow->GetUsername() == Username)
				{
					VerticalBox_RedTeam->RemoveChild(PlayerListRow);

					VerticalBox_BlueTeam->AddChildToVerticalBox(PlayerListRow);			
				}
			}
		}
	}
}

void UTDMPlayerList::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_RedTeam->OnClicked.AddDynamic(this, &ThisClass::OnRedTeamButtonClicked);
	Button_BlueTeam->OnClicked.AddDynamic(this, &ThisClass::OnBlueTeamButtonClicked);
}

void UTDMPlayerList::OnRedTeamButtonClicked()
{
	if (ALobbyPlayerController* LobbyPC = GetOwningPlayer<ALobbyPlayerController>())
	{
		if (ABasePlayerState* BasePS = GetOwningPlayerState<ABasePlayerState>())
		{
			ETeam CurrentTeam = BasePS->GetTeam();
			if (CurrentTeam == ETeam::BlueTeam)
			{
				ETeam NewTeam = ETeam::RedTeam;
				LobbyPC->ServerChangePlayerTeam(PlayerUsername, NewTeam, BasePS);
			}
		}
	}
}

void UTDMPlayerList::OnBlueTeamButtonClicked()
{
	if (ALobbyPlayerController* LobbyPC = GetOwningPlayer<ALobbyPlayerController>())
	{
		if (ABasePlayerState* BasePS = GetOwningPlayerState<ABasePlayerState>())
		{
			ETeam CurrentTeam = BasePS->GetTeam();
			if (CurrentTeam == ETeam::RedTeam)
			{
				ETeam NewTeam = ETeam::BlueTeam;
				LobbyPC->ServerChangePlayerTeam(PlayerUsername, NewTeam, BasePS);
			}
		}
	}
}
