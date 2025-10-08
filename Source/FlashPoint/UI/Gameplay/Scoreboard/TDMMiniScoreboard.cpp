// Fill out your copyright notice in the Description page of Project Settings.


#include "TDMMiniScoreboard.h"

#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TDMMiniScoreboard)

void UTDMMiniScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UpdateKillCount(ETeam::RedTeam, 0);
	UpdateKillCount(ETeam::BlueTeam, 0);
}

void UTDMMiniScoreboard::UpdateKillCount(ETeam Team, int32 KillCount)
{
	if (Team == ETeam::RedTeam)
	{
		Text_RedKillCount->SetText(FText::AsNumber(KillCount));
	}
	else if (Team == ETeam::BlueTeam)
	{
		Text_BlueKillCount->SetText(FText::AsNumber(KillCount));
	}
}
