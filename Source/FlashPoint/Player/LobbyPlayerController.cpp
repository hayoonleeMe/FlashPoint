// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

#include "BasePlayerState.h"
#include "Game/LobbyGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LobbyPlayerController)

void ALobbyPlayerController::ServerChangePlayerTeam_Implementation(const FString& Username, ETeam NewTeam, ABasePlayerState* BasePS)
{
	if (BasePS)
	{
		BasePS->SetTeam(NewTeam);
	}
}

void ALobbyPlayerController::ServerStartMatch_Implementation(EMatchMode MatchMode)
{
	if (GetWorld())
	{
		if (ALobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
		{
			LobbyGameMode->StartMatch(MatchMode);
		}
	}
}
