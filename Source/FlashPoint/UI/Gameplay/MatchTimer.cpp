// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchTimer.h"

#include "Components/TextBlock.h"
#include "Game/FPGameState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MatchTimer)

void UMatchTimer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UWorld* World = GetWorld())
	{
		if (AFPGameState* FPGameState = World->GetGameState<AFPGameState>())
		{
			CachedGameState = FPGameState;
			FPGameState->OnClientMatchEndTimeReplicatedDelegate.AddUObject(this, &ThisClass::OnClientMatchEndTimeReplicated);
		}
	}
}

void UMatchTimer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// MatchEndTime 설정된 후
	if (MatchEndTime > 0.f && CachedGameState.IsValid())
	{
		const int32 RemainingTimeSeconds = FMath::CeilToInt(MatchEndTime - CachedGameState->GetServerWorldTimeSeconds());
		if (RemainingTimeSeconds > 0)
		{
			const int32 Minutes = RemainingTimeSeconds / 60;
			const int32 Seconds = RemainingTimeSeconds % 60;
			if (Seconds != LastSeconds)
			{
				// 변경이 있을 때만 업데이트
				LastSeconds = Seconds;
				const FString TimeStr = FString::Printf(TEXT("%02d : %02d"), Minutes, Seconds);
				Text_Time->SetText(FText::FromString(TimeStr));
			}
		}
		else
		{
			// Timer 업데이트 종료
			MatchEndTime = 0.f;
			LastSeconds = 0;
			const FString TimeStr = TEXT("00 : 00");
			Text_Time->SetText(FText::FromString(TimeStr));
		}
	}
}

void UMatchTimer::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text_Time->SetVisibility(ESlateVisibility::Hidden);
}

void UMatchTimer::OnClientMatchEndTimeReplicated(float InMatchEndTime)
{
	MatchEndTime = InMatchEndTime;
	if (MatchEndTime > 0.f)
	{
		Text_Time->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Text_Time->SetVisibility(ESlateVisibility::Hidden);
	}
}
