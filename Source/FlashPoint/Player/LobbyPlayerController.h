// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "Data/MatchTypes.h"
#include "LobbyPlayerController.generated.h"

class ABasePlayerState;

UCLASS()
class FLASHPOINT_API ALobbyPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

public:
	// BasePS의 Team을 NewTeam으로 변경하고, LobbyWidget에 업데이트하는 Server RPC
	UFUNCTION(Server, Reliable)
	void ServerChangePlayerTeam(const FString& Username, ETeam NewTeam, ABasePlayerState* BasePS);

	// MatchMode에 해당하는 매치를 시작한다.
	UFUNCTION(Server, Reliable)
	void ServerStartMatch(EMatchMode MatchMode);
};
