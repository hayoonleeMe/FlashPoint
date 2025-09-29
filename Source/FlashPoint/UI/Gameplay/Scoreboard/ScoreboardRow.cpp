// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreboardRow.h"

#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ScoreboardRow)

void UScoreboardRow::SetRow(const FString& Username, int32 KillCount, int32 DeathCount, bool bIsLocalPlayer) const
{
	ShowRow(true);
	
	Text_Username->SetText(FText::FromString(Username));
	Text_Kill->SetText(FText::AsNumber(KillCount));
	Text_Death->SetText(FText::AsNumber(DeathCount));

	// 플레이어를 나타내는 Row면 텍스트 강조 표시
	const FLinearColor Color = bIsLocalPlayer ? HighlightColor : FLinearColor::White;
	Text_Username->SetColorAndOpacity(Color);
	Text_Kill->SetColorAndOpacity(Color);
	Text_Death->SetColorAndOpacity(Color);
}

void UScoreboardRow::ShowRow(bool bShow) const
{
	ESlateVisibility NewVisibility = bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	Text_Username->SetVisibility(NewVisibility);
	Text_Kill->SetVisibility(NewVisibility);
	Text_Death->SetVisibility(NewVisibility);
}
