// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * Lobby에서 사용할 GameMode
 */
UCLASS()
class FLASHPOINT_API ALobbyGameMode : public ABaseGameMode
{
	GENERATED_BODY()

public:
	// MatchMode에 해당하는 레벨로 이동해 매치를 시작한다.
	void TravelToMatchLevel(EMatchMode MatchMode);
};
