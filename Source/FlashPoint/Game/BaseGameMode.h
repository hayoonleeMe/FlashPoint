// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/MatchTypes.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

class ABasePlayerState;

/**
 * 매치 정보와 서버에 접속한 플레이어 정보를 관리하는 GameMode
 */
UCLASS()
class FLASHPOINT_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABaseGameMode();

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	virtual void Logout(AController* Exiting) override;

	virtual void PostSeamlessTravel() override;

protected:
	virtual void BeginPlay() override;

	// 현재 매치 관련 정보를 저장하는 데이터
	// 이 데이터는 GameLift SDK의 콜백 함수를 통해 가져오기 때문에, 내부 데이터 CurrentPlayers의 초기값은 0이다.
	FMatchInfo MatchInfo;

	// Red Team 플레이어 배열
	UPROPERTY()
	TArray<TWeakObjectPtr<ABasePlayerState>> RedTeamPlayers;

	// Blue Team 플레이어 배열
	UPROPERTY()
	TArray<TWeakObjectPtr<ABasePlayerState>> BlueTeamPlayers;

	// BasePS 플레이어의 팀을 결정한다.
	// Team Death Match에서만 수행한다.
	void ChoosePlayerTeam(ABasePlayerState* BasePS);

	// 플레이어 팀이 변경될 때 호출되는 콜백 함수
	void OnPlayerTeamChanged(ABasePlayerState* BasePS);

	// GameLift Game Session이 시작될 때 호출되는 콜백함수
	void OnGameSessionStarted(const FGameSessionInfo& GameSessionInfo);

	// GameLift Game Session이 종료될 때 호출되는 콜백함수
	void OnGameSessionTerminated();

	// 모든 플레이어를 메인 메뉴로 퇴장시킨다.
	void KickAllPlayers() const;

	// 서버에서 호스트 플레이어(방장)이 남아있는지 여부를 반환한다.
	bool IsHostStillRemain() const;
};
