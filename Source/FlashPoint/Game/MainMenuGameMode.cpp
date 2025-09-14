// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"

#include "Data/MatchTypes.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MainMenuGameMode)

FString AMainMenuGameMode::GetParsedKickReasonOption() const
{
	return UGameplayStatics::ParseOption(OptionsString, KickReasonOption);
}
