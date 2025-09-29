// Fill out your copyright notice in the Description page of Project Settings.


#include "RankScoreboardRow.h"

#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RankScoreboardRow)

void URankScoreboardRow::SetRow(int32 Rank, const FString& Username, int32 KillCount, int32 DeathCount, bool bIsLocalPlayer) const
{
	ShowRow(true);

	Text_Rank->SetText(FText::AsNumber(Rank));
	Text_Username->SetText(FText::FromString(Username));
	Text_Kill->SetText(FText::AsNumber(KillCount));
	Text_Death->SetText(FText::AsNumber(DeathCount));

	// 플레이어를 나타내는 Row면 텍스트 강조 표시
	const FLinearColor Color = bIsLocalPlayer ? HighlightColor : FLinearColor::White;
	Text_Rank->SetColorAndOpacity(Color);
	Text_Username->SetColorAndOpacity(Color);
	Text_Kill->SetColorAndOpacity(Color);
	Text_Death->SetColorAndOpacity(Color);
}

void URankScoreboardRow::ShowRow(bool bShow) const
{
	Super::ShowRow(bShow);

	Text_Rank->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
