// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchResult.h"

#include "Components/TextBlock.h"
#include "Game/FPGameState.h"
#include "Player/FPPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MatchResult)

UMatchResult::UMatchResult(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	VictoryText = TEXT("VICTORY!");
	VictoryColor = FLinearColor(1.f, 0.752941f, 0.129412f);
	
	DefeatText = TEXT("DEFEAT");
	DefeatColor = FLinearColor::Red;
	
	DrawText = TEXT("DRAW");
	DrawColor = FLinearColor::White;
	
	DefaultRankColor = FLinearColor::White;
}

void UMatchResult::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (AFPPlayerState* FPPlayerState = GetOwningPlayerState<AFPPlayerState>())
	{
		if (UWorld* World = GetWorld())
		{
			if (AFPGameState* FPGameState = World->GetGameState<AFPGameState>())
			{
				const FMatchInfo& MatchInfo = FPGameState->GetMatchInfo();
				if (MatchInfo.MatchMode == EMatchMode::TeamDeathMatch)
				{
					InitializeTDM(FPPlayerState->GetTeam(), FPGameState->GetWinningTeam());
				}
				else if (MatchInfo.MatchMode == EMatchMode::FreeForAll)
				{
					InitializeFFA(FPGameState->GetUserRank(FPPlayerState->GetUsername()));
				}
			}
		}	
	}

	PlayAnimationForward(FadeIn);
}

void UMatchResult::InitializeTDM(ETeam UserTeam, ETeam WinningTeam)
{
	if (WinningTeam == ETeam::None)
	{
		// Draw
		Text_Result->SetText(FText::FromString(DrawText));
		Text_Result->SetColorAndOpacity(DrawColor);
	}
	else
	{
		const bool bIsWinner = UserTeam == WinningTeam;
		Text_Result->SetText(FText::FromString(bIsWinner ? VictoryText : DefeatText));
		Text_Result->SetColorAndOpacity(bIsWinner ? VictoryColor : DefeatColor);
	}
}

void UMatchResult::InitializeFFA(int32 UserRank)
{
	Text_Result->SetText(FText::FromString(GetOrdinalString(UserRank)));
					
	const bool bIsWinner = UserRank == 1;
	Text_Result->SetColorAndOpacity(bIsWinner ? VictoryColor : DefaultRankColor);
}
