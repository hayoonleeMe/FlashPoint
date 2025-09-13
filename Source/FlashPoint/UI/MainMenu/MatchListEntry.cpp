// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchListEntry.h"

#include "MatchListObject.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MatchListEntry)

void UMatchListEntry::HighlightEntry(bool bHighlight) const
{
	if (bHighlight)
	{
		Border->SetBrushColor(SelectedColor);
	}
	else
	{
		Border->SetBrushColor(NormalColor);
	}
}

void UMatchListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (UMatchListObject* Object = Cast<UMatchListObject>(ListItemObject))
	{
		Text_RoomName->SetText(FText::FromString(Object->RoomName));
		Text_MatchMode->SetText(FText::FromString(Object->MatchModeStr));
		FString PlayerCount = FString::Printf(TEXT("%s / %s"), *Object->CurrentPlayers, *Object->MaxPlayers);
		Text_PlayerCount->SetText(FText::FromString(PlayerCount));
	}
}

void UMatchListEntry::NativeOnEntryReleased()
{
	Border->SetBrushColor(NormalColor);
}

void UMatchListEntry::NativePreConstruct()
{
	Super::NativePreConstruct();

	Border->SetBrushColor(NormalColor);
}
