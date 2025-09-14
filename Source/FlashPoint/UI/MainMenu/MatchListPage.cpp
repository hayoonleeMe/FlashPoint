// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchListPage.h"

#include "MatchListEntry.h"
#include "MatchListObject.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Data/MatchTypes.h"
#include "System/OnlineServiceSubsystem.h"
#include "System/PlayerAuthSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MatchListPage)

void UMatchListPage::InitializeWidget()
{
	ListView->ClearListItems();
	UnselectMatchList();
	
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (!OnlineServiceSubsystem->OnCreatePlayerSessionSucceededDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->OnCreatePlayerSessionSucceededDelegate.AddUObject(this, &ThisClass::OnJoinSucceeded);
		}
		OnlineServiceSubsystem->DescribeGameSessions();
	}
}

void UMatchListPage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (!OnlineServiceSubsystem->DescribeGameSessionsStatusMessageDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->DescribeGameSessionsStatusMessageDelegate.AddUObject(this, &ThisClass::UpdateRefreshStatusMessage);
		}
		if (!OnlineServiceSubsystem->CreatePlayerSessionStatusMessageDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->CreatePlayerSessionStatusMessageDelegate.AddUObject(this, &ThisClass::UpdateJoinStatusMessage);
		}
		if (!OnlineServiceSubsystem->OnDescribeGameSessionsSucceededDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->OnDescribeGameSessionsSucceededDelegate.AddUObject(this, &ThisClass::OnDescribeGameSessionsSucceeded);
		}
	}

	ListView->OnItemClicked().AddUObject(this, &ThisClass::OnMatchListItemClicked);
	Button_Join->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
	Button_Back->OnClicked.AddDynamic(this, &ThisClass::OnBackButtonClicked);
	Button_Refresh->OnClicked.AddDynamic(this, &ThisClass::OnRefreshButtonClicked);
}

void UMatchListPage::OnDescribeGameSessionsSucceeded(const TArray<FGameSessionInfo>& GameSessionInfos)
{
	ListView->ClearListItems();
	
	for (const FGameSessionInfo& GameSessionInfo : GameSessionInfos)
	{
		const FMatchInfo MatchInfo(GameSessionInfo);
		
		UMatchListObject* Object = NewObject<UMatchListObject>(ListView, UMatchListObject::StaticClass());
		Object->GameSessionId = MatchInfo.GameSessionId;
		Object->RoomName = MatchInfo.RoomName;
		Object->MatchModeStr = LexToString(MatchInfo.MatchMode);
		Object->MaxPlayers = MatchInfo.MaxPlayers;
		Object->CurrentPlayersStr = FString::FromInt(MatchInfo.CurrentPlayers);
		ListView->AddItem(Object);
	}
}

void UMatchListPage::OnMatchListItemClicked(UObject* Item)
{
	UnselectMatchList();
	SelectMatchList(Cast<UMatchListObject>(Item));
}

void UMatchListPage::UnselectMatchList()
{
	if (UMatchListEntry* LastSelectedEntry = ListView->GetEntryWidgetFromItem<UMatchListEntry>(SelectedMatchList.Get()))
	{
		LastSelectedEntry->HighlightEntry(false);
	}
	SelectedMatchList = nullptr;
	Button_Join->SetIsEnabled(false);
}

void UMatchListPage::SelectMatchList(UMatchListObject* Item)
{
	if (Item)
	{
		if (UMatchListEntry* CurrentSelectedEntry = ListView->GetEntryWidgetFromItem<UMatchListEntry>(Item))
		{
			CurrentSelectedEntry->HighlightEntry(true);
		}
		SelectedMatchList = Item;
		Button_Join->SetIsEnabled(true);
	}
}

void UMatchListPage::OnJoinButtonClicked()
{
	if (SelectedMatchList.IsValid())
	{
		Button_Join->SetIsEnabled(false);
		
		if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
		{
			if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
			{
				OnlineServiceSubsystem->CreatePlayerSession(PlayerAuthSubsystem->GetUsername(), SelectedMatchList->GameSessionId);
			}
		}
	}
}

void UMatchListPage::OnJoinSucceeded(const FString& URL)
{
	// 매치의 서버 주소로 접속
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->ClientTravel(URL, TRAVEL_Absolute);
	}
}

void UMatchListPage::OnBackButtonClicked()
{
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		OnlineServiceSubsystem->OnCreatePlayerSessionSucceededDelegate.RemoveAll(this);
	}
}

void UMatchListPage::OnRefreshButtonClicked()
{
	InitializeWidget();
	Button_Refresh->SetIsEnabled(false);
}

void UMatchListPage::UpdateRefreshStatusMessage(const FString& Message, bool bShouldResetWidget)
{
	Text_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidget)
	{
		Button_Refresh->SetIsEnabled(true);
	}
}

void UMatchListPage::UpdateJoinStatusMessage(const FString& Message, bool bShouldResetWidget)
{
	Text_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidget && SelectedMatchList.IsValid())
	{
		Button_Join->SetIsEnabled(true);
	}
}
