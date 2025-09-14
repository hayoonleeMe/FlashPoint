// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MatchListObject.generated.h"

/**
 * UMatchListPage의 ListView에 항목을 추가할 때 사용할 Object
 */
UCLASS()
class FLASHPOINT_API UMatchListObject : public UObject
{
	GENERATED_BODY()

public:
	FString GameSessionId;
	FString RoomName;
	FString MatchModeStr;
	FString MaxPlayers;
	FString CurrentPlayersStr;
};
