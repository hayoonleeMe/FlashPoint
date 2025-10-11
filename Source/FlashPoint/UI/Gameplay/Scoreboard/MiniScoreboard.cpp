// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniScoreboard.h"

#include "Components/TextBlock.h"
#include "Game/FPGameState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MiniScoreboard)

void UMiniScoreboard::SetGoalKillCount(int32 GoalKillCount)
{
	Text_GoalKillCount->SetText(FText::AsNumber(GoalKillCount));
}

void UMiniScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UWorld* World = GetWorld())
	{
		if (AFPGameState* FPGameState = World->GetGameState<AFPGameState>())
		{
			FPGameState->OnTeamKillCountUpdatedDelegate.AddUObject(this, &ThisClass::UpdateKillCount);
		}
	}
}
