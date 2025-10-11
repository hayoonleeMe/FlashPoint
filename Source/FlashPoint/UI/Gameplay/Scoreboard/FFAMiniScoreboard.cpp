// Fill out your copyright notice in the Description page of Project Settings.


#include "FFAMiniScoreboard.h"

#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FFAMiniScoreboard)

void UFFAMiniScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UpdateKillCount(ETeam::None, 0);
}

void UFFAMiniScoreboard::UpdateKillCount(ETeam Team, int32 KillCount)
{
	if (Team == ETeam::None)
	{
		Text_KillCount->SetText(FText::AsNumber(KillCount));
	}
}
