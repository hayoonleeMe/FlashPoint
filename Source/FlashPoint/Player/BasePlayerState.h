// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/MatchTypes.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

class ABasePlayerState;

// 서버에서 플레이어의 팀이 변경될 때 브로드캐스트하는 델레게이트
// 유효한 팀에서 다른 유효한 팀으로 변경될 때만 브로드캐스트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnServerPlayerTeamChangedDelegate, ABasePlayerState*/*BasePS*/)

/**
 * 서버에 접속한 플레이어 정보를 저장하고, seamless travel 시 데이터를 유지하는 Player State
 */
UCLASS()
class FLASHPOINT_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABasePlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OverrideWith(APlayerState* PlayerState) override;

	// 서버에서만 브로드캐스트한다.
	FOnServerPlayerTeamChangedDelegate OnServerPlayerTeamChangedDelegate;
	
	void SetTeam(ETeam InTeam);
	ETeam GetTeam() const { return Team; }

	void SetServerPlayerSessionId(const FString& InPlayerSessionId) { ServerPlayerSessionId = InPlayerSessionId; }
	FString GetServerPlayerSessionId() const { return ServerPlayerSessionId; }
	
	void SetServerUsername(const FString& InUsername) { ServerUsername = InUsername; }
	FString GetServerUsername() const { return ServerUsername; }

protected:
	// TDM인 경우 설정되는 팀 데이터
	UPROPERTY(VisibleAnywhere, Replicated)
	ETeam Team;

	// Player Session을 생성하고 서버에 접속한 플레이어의 Player Session Id
	// 서버에서만 설정된다.
	FString ServerPlayerSessionId;

	// 서버에 접속한 플레이어의 Username
	// 서버에서만 설정된다.
	FString ServerUsername;
};
