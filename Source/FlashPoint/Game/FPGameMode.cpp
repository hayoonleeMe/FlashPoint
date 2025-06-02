// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameMode.h"

#include "Character/FPCharacter.h"
#include "Player/FPPlayerController.h"
#include "Player/FPPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameMode)

AFPGameMode::AFPGameMode()
{
	DefaultPawnClass = AFPCharacter::StaticClass();
	PlayerControllerClass = AFPPlayerController::StaticClass();
	PlayerStateClass = AFPPlayerState::StaticClass();
}
