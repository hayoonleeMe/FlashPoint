// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/OnlineServiceTypes.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PlayerAuthSubsystem.generated.h"

class IOnlineServiceInterface;
struct FAuthenticationResult;

/**
 * 플레이어의 Authentication 인증 정보를 관리하는 Local Player Subsystem
 */
UCLASS()
class ONLINESERVICE_API UPlayerAuthSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	static UPlayerAuthSubsystem* Get(const UObject* WorldContextObject);

	// 플레이어 접속 시 인증 정보를 초기화한다.
	void InitializeTokens(const FString& InUsername, const FString& InEmail, const FAuthenticationResult& AuthResult, const TScriptInterface<IOnlineServiceInterface>& InOnlineServiceInterface);
	
	// Refresh된 토큰을 업데이트한다.
	void UpdateTokens(const FString& AccessToken, const FString& IdToken);

	FString GetUsername() const { return Username; }
	FString GetEmail() const { return Email; }
    FAuthenticationResult GetAuthResult() const { return AuthenticationResult; }
	
private:
	// 접속한 플레이어의 Username
	FString Username;

	// 접속한 플레이어의 Email
    FString Email;
    
    // 접속한 플레이어의 인증 정보
    FAuthenticationResult AuthenticationResult;

	UPROPERTY()
	TScriptInterface<IOnlineServiceInterface> OnlineServiceInterface;

	// TokenRefreshInterval이 지나면 Auth Token Refresh를 시도하는 타이머를 시작
	void SetRefreshTokenTimer();

	// AccessToken과 IdToken을 Refresh한다.
	void RefreshTokens();

	// Auth Token 만료 시간 (45분)
    float TokenRefreshInterval = 2700.f;
    FTimerHandle RefreshTimer;
};
