// Fill out your copyright notice in the Description page of Project Settings.


#include "Scoreboard.h"

#include "System/PlayerAuthSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Scoreboard)

void UScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
	{
		PlayerUsername = PlayerAuthSubsystem->GetUsername();
	}
}
