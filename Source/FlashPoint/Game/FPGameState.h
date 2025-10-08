// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "FPGameState.generated.h"

// 서버에서 매치가 시작된 뒤, 매치 종료 시간을 나타내는 MatchEndTime이 클라이언트에서 복제됨을 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnClientMatchEndTimeReplicatedDelegate, float/*MatchEndTime*/);

// Team의 KillCount가 업데이트될 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTeamKillCountUpdatedDelegate, ETeam/*Team*/, int32/*KillCount*/);

/**
 * 게임플레이에서 사용할 GameState
 */
UCLASS()
class FLASHPOINT_API AFPGameState : public ABaseGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnClientMatchEndTimeReplicatedDelegate OnClientMatchEndTimeReplicatedDelegate;
	FOnTeamKillCountUpdatedDelegate OnTeamKillCountUpdatedDelegate;

	// GameMode에서 전달한 MatchEndTime을 설정한다.
	void SetMatchEndTime(float InMatchEndTime);


protected:
	virtual void BeginPlay() override;
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
};
