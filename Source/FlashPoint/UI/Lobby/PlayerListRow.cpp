// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerListRow.h"

#include "Components/TextBlock.h"

void UPlayerListRow::SetUsername(const FString& InUsername)
{
	Username = InUsername;
	Text_Username->SetText(FText::FromString(Username));
}

void UPlayerListRow::HighlightUsername() const
{
	Text_Username->SetColorAndOpacity(HighlightColor);
}
