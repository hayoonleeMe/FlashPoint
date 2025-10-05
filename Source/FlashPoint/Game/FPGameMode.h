// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameMode.h"
#include "FPGameMode.generated.h"

class ATeamPlayerStart;

/**
 * USTRUCT Wrapper for TArray
 */
USTRUCT()
struct FTeamPlayerStartArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<ATeamPlayerStart>> TeamPlayerStarts;
};

/**
 * 게임플레이에서 사용할 GameModeBase
 */
UCLASS()
class FLASHPOINT_API AFPGameMode : public ABaseGameMode
{
	GENERATED_BODY()

public:
	AFPGameMode();
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	// 레벨에 존재하는 모든 ATeamPlayerStart 액터를 캐싱한다.
	void InitializeTeamPlayerStarts();

	// 팀 별로 ATeamPlayerStart 액터 배열을 저장하는 맵
	UPROPERTY()
	TMap<ETeam, FTeamPlayerStartArray> TeamPlayerStartsMap;

	// ============================================================================
	// Match
	// ============================================================================
protected:
	virtual void HandleMatchHasStarted() override;

private:
	// 매치를 진행할 시간(초)
	UPROPERTY(EditDefaultsOnly, Category="Match")
	float MatchTime;
};
