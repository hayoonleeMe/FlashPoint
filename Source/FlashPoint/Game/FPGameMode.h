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
	virtual void HandleMatchHasEnded() override;

private:
	// 매치를 진행할 시간(초)
	UPROPERTY(EditDefaultsOnly, Category="Match")
	float MatchTime;

	// 매치가 종료된 후, 로비로 이동할 때까지 기다릴 시간
	UPROPERTY(EditDefaultsOnly, Category="Match")
	float MatchEndDelay;

	// 매치가 종료될 때 슬로우 모션 강도
	UPROPERTY(EditDefaultsOnly, Category="Match", meta=(ClampMin="0.1", ClampMax="1.0"))
	float MatchEndTimeDilation;

	// 모든 플레이어를 로비로 이동시킨다.
	void TravelToLobby() const;
};
