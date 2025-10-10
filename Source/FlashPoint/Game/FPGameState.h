// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "FPGameState.generated.h"

// 서버에서 매치가 시작된 뒤, 매치 종료 시간을 나타내는 MatchEndTime이 클라이언트에서 복제됨을 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnClientMatchEndTimeReplicatedDelegate, float/*MatchEndTime*/);

// Team의 KillCount가 업데이트될 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTeamKillCountUpdatedDelegate, ETeam/*Team*/, int32/*KillCount*/);

// MatchState가 변경됨을 알리는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnMatchStateChangedDelegate);

/**
 * 게임플레이에서 사용할 GameState
 */
UCLASS()
class FLASHPOINT_API AFPGameState : public ABaseGameState
{
	GENERATED_BODY()

public:
	AFPGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnClientMatchEndTimeReplicatedDelegate OnClientMatchEndTimeReplicatedDelegate;
	FOnTeamKillCountUpdatedDelegate OnTeamKillCountUpdatedDelegate;
	
	FOnMatchStateChangedDelegate OnMatchEndedDelegate;
	FOnMatchStateChangedDelegate OnMatchEndTimeDilationFinishedDelegate;

	// GameMode에서 전달한 MatchEndTime을 설정한다.
	void SetMatchEndTime(float InMatchEndTime);

	// TimeDilation 강도만큼 서버와 클라이언트에 슬로우 모션을 적용한다.
	void SetGlobalTimeDilation(float TimeDilation);

	// 매치를 이긴 팀을 반환한다.
	// MatchMode가 FreeForAll이거나, 비겼으면 ETeam::None을 반환한다.
	ETeam GetWinningTeam() const;

	// Username에 해당하는 유저의 순위를 반환한다.
	int32 GetUserRank(const FString& Username);

protected:
	virtual void BeginPlay() override;
	virtual void HandleMatchHasEnded() override;

private:
	// 팀 별 합산 KillCount
	TMap<ETeam, int32> TeamKillCounts;

	// 플레이어 별 KillCount
	// Key: Username, Value: KillCount
	TMap<FName, int32> PlayerKillCounts;

	// 클라이언트에 PlayerInfo 추가가 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoAdded(const FPlayerInfo& PlayerInfo);
	
	// 클라이언트에 PlayerInfo 제거가 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoRemoved(const FPlayerInfo& PlayerInfo);

	// 클라이언트에 PlayerInfo 변경이 Replicate될 떄 호출되는 Callback
	void OnClientPlayerInfoChanged(const FPlayerInfo& PlayerInfo);
	
	// 매치가 종료되는 WorldTimeSeconds을 나타낸다.
	// 서버에서 매치를 시작한 뒤 설정된다.
	UPROPERTY(ReplicatedUsing=OnRep_MatchEndTime)
	float MatchEndTime;
	
	UFUNCTION()
	void OnRep_MatchEndTime();

	// 전체 월드에 적용하는 슬로우 모션 강도
	UPROPERTY(ReplicatedUsing=OnRep_GlobalTimeDilation)
	float GlobalTimeDilation;

	UFUNCTION()
	void OnRep_GlobalTimeDilation();

	// 매치가 종료된 후, 슬로우 모션을 수행할 시간
	UPROPERTY(EditDefaultsOnly, Category="Match")
	float MatchEndTimeDilationDelay;

	// 매치가 종료된 후 슬로우 모션이 종료될 때 호출되는 Callback
	void OnMatchEndTimeDilationFinished();
};
