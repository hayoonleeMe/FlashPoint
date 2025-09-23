// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/PlayerInfo.h"
#include "Data/MatchTypes.h"
#include "GameFramework/GameState.h"
#include "BaseGameState.generated.h"

// 클라이언트에서 MatchInfo 프로퍼티가 복제됐을 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnClientMatchInfoReplicatedDelegate, const FMatchInfo&/*MatchInfo*/);

// 클라이언트에서 PlayerInfoArray가 변경될 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnClientPlayerInfoArrayDelegate, const FString&/*Username*/, ETeam/*Team*/);

/**
 * 매치 정보와 서버에 접속한 플레이어 정보를 관리하는 GameState
 */
UCLASS()
class FLASHPOINT_API ABaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// MatchInfo를 저장한다.
	void SetMatchInfo(const FMatchInfo& InMatchInfo);

	// 새로 접속한 플레이어 정보를 추가한다.
	void AddPlayerInfo(const FString& Username, ETeam Team);

	// 퇴장한 플레이어 정보를 제거한다.
	void RemovePlayerInfo(const FString& Username);

	// 플레이어 정보를 변경한다.
	void UpdatePlayerInfo(const FString& Username, ETeam Team);

	FOnClientMatchInfoReplicatedDelegate OnClientMatchInfoReplicatedDelegate;

	// PlayerInfo가 추가될 때 브로드캐스트
	FOnClientPlayerInfoArrayDelegate OnClientPlayerInfoAddedDelegate;

	// PlayerInfo가 제거될 때 브로드캐스트
	FOnClientPlayerInfoArrayDelegate OnClientPlayerInfoRemovedDelegate;

	// PlayerInfo가 변경될 때 브로드캐스트
	FOnClientPlayerInfoArrayDelegate OnClientPlayerInfoChangedDelegate;

protected:
	virtual void BeginPlay() override;
	
	// 현재 매치 관련 정보를 저장하는 데이터
	UPROPERTY(ReplicatedUsing=OnRep_MatchInfo);
	FMatchInfo MatchInfo;

	UFUNCTION()
	void OnRep_MatchInfo();

	// 로컬 플레이어의 Username
	FString PlayerUsername;

	// 실제 서버에 접속 중인 플레이어 정보를 저장하는 배열
	// 서버에서만 변경되어야 한다.
	UPROPERTY(ReplicatedUsing=OnRep_PlayerInfoArray);
	FPlayerInfoArray PlayerInfoArray;

	// 모든 종류의 변경된 플레이어 정보를 처리한다.
	UFUNCTION()
	void OnRep_PlayerInfoArray();

	// 새로 추가된 플레이어 정보를 위젯에 업데이트한다. 
	void HandleAddedPlayerInfos(TArray<FPlayerInfo>& AddedPlayerInfos) const;

	// 제거된 플레이어 정보를 위젯에 업데이트한다.
	void HandleRemovedPlayerInfos(TArray<FPlayerInfo>& RemovedPlayerInfos) const;

	// 변경된 플레이어 정보를 위젯에 업데이트한다.
	void HandleChangedPlayerInfos(TArray<FPlayerInfo>& ChangedPlayerInfos) const;
};
