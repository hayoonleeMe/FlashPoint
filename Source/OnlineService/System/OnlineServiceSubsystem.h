// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineServiceInterface.h"
#include "Data/OnlineServiceTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineServiceSubsystem.generated.h"

struct FGameplayTag;
struct FServerParameters;
struct FProcessParameters;
class UAPIData;

// API 호출이 성공했을 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnAPIRequestSucceededDelegate);

// API 호출이 성공했을 때 하나의 FString 데이터를 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAPIRequestSucceededWithOneStringDelegate, const FString&/*Payload*/);

// API 호출 상태를 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FAPIStatusMessageDelegate, const FString&/*StatusMessage*/, bool/*bShouldResetWidget*/);

// GameLift Server SDK의 프로세스 상태 변경 콜백 함수가 호출될 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE(FOnGameLiftServerProcessUpdatedDelegate);

// GameLift Server SDK의 OnStartGameSession 델레게이트의 콜백 함수가 호출될 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnServerStartGameSessionDelegate, const FGameSessionInfo&/*GameSessionInfo*/);

// DescribeGameSessions API 결과를 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDescribeGameSessionsSucceededDelegate, const TArray<FGameSessionInfo>&/*GameSessionInfos*/);

/**
 * AWS와 연동하는 기능을 구현, 관리하는 Subsystem
 */
UCLASS(Blueprintable)
class ONLINESERVICE_API UOnlineServiceSubsystem : public UGameInstanceSubsystem, public IOnlineServiceInterface
{
	GENERATED_BODY()

public:
	UOnlineServiceSubsystem();
	
	static UOnlineServiceSubsystem* Get(const UObject* WorldContextObject);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Params에 저장된 Json을 FString으로 변환해 반환한다.
	static FString SerializeJsonContent(const TMap<FString, FString>& Params);

	// JsonObject의 MetaData 영역의 값을 출력한다.
	static void DumpMetaData(const TSharedPtr<FJsonObject>& JsonObject);

	// JsonObject에 오류를 나타내는 값이 있는지 체크하고, 있다면 OutExceptionName에 오류를 저장한다.
	static bool ContainsErrors(const TSharedPtr<FJsonObject>& JsonObject, FString& OutExceptionName, FString& OutErrorMessage);

	// API Response를 OutStruct로 변환하고, 결과가 유효한지를 반환한다.
	// OutExceptionName, OutErrorMessage가 nullptr가 아니라면, Response가 Error를 포함할 때 설정된다.
	template<typename T>
	bool ParseAPIResponse(FHttpResponsePtr Response, const FString& APINameToLog, T& OutStruct, FString* OutExceptionName, FString* OutErrorMessage);

	// API Response가 유효한지를 반환한다.
	// OutExceptionName, OutErrorMessage가 nullptr가 아니라면, Response가 Error를 포함할 때 설정된다.
	bool ParseAPIResponse(FHttpResponsePtr Response, const FString& APINameToLog, FString* OutExceptionName, FString* OutErrorMessage);

	// API Response를 OutJsonObject로 변환하고, 결과가 유효한지를 반환한다.
	// OutExceptionName, OutErrorMessage가 nullptr가 아니라면, Response가 Error를 포함할 때 설정된다.
	bool ParseAPIResponse(FHttpResponsePtr Response, const FString& APINameToLog, TSharedPtr<FJsonObject>& OutJsonObject, FString* OutExceptionName, FString* OutErrorMessage);

	// ============================================================================
	// PlayerSign
	// ============================================================================
	// 플레이어가 수행하는 Aws PlayerSign API 관련 기능
	// 서버 접속 전 Standalone, 서버 접속 후 Client
public:
	// IOnlineServiceInterface
	virtual void RefreshTokens(const FString& RefreshToken) override;
	void SignIn(const FString& Username, const FString& Password);
	void SignUp(const FString& Username, const FString& Password, const FString& Email);
	void CheckUserAndResendConfirmCode(const FString& Username);
	void ConfirmSignUp(const FString& ConfirmationCode);
	void SignOut(const FString& AccessToken);

	FOnAPIRequestSucceededDelegate OnSignInSucceededDelegate;
	
	// CodeDeliveryDestination String을 브로드캐스트한다.
	FOnAPIRequestSucceededWithOneStringDelegate OnSignUpSucceededDelegate;

	FOnAPIRequestSucceededDelegate OnConfirmSignUpSucceededDelegate;

	FOnAPIRequestSucceededDelegate OnSignOutSucceededDelegate;

	FAPIStatusMessageDelegate SignInStatusMessageDelegate;

	FAPIStatusMessageDelegate SignUpStatusMessageDelegate;

	FAPIStatusMessageDelegate ConfirmSignUpStatusMessageDelegate;

private:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerSign")
	TObjectPtr<UAPIData> PlayerSignAPIData;

	// 최근에 접속한 유저 이름
	FString LastUsername;

	void RefreshTokens_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString Username);
	void CheckUserAndResendConfirmCode_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ConfirmSignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SignOut_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

public:
	// 로그인 시 입력하는 문자열들이 모두 유효한지 체크한다.
	bool ValidateSignInInput(const FString& Username, const FString& Password) const;
	
	// 가입 시 입력하는 문자열들이 모두 유효한지 체크한다.
	bool ValidateSignUpInput(const FString& Username, const FString& Password, const FString& ConfirmPassword, const FString& Email) const;

	// Email이 유효한지 체크한다.
	static bool IsValidEmail(const FString& Email);

	// Password가 유효한지 체크한다.
	// 유효하지 않다면, OutStatusMessage를 이유를 설명하는 문자열로 설정한다. 
	static bool IsValidPassword(const FString& Password, FString& OutStatusMessage);

	// 비밀번호 최소 길이
	UPROPERTY(EditDefaultsOnly, Category = "PlayerSign")
	int32 MinimumPasswordLength;

	// 가입 시 입력한 Confirmation Code가 유효한지 체크한다.
	bool ValidateConfirmationCode(const FString& Code) const;

	// ============================================================================
	// GameLift Server
	// ============================================================================
	// 서버 프로세스 관련 기능들 (dedicated server only)
public:
	void InitGameLift();

	static void TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& OutErrorMessage);
	static void RemovePlayerSession(const FString& PlayerSessionId);

	FOnServerStartGameSessionDelegate OnServerStartGameSessionDelegate;

	FOnGameLiftServerProcessUpdatedDelegate OnProcessTerminateDelegate;

	FOnGameLiftServerProcessUpdatedDelegate OnHealthCheckDelegate;
	
private:
	// 서버에서 InitGameLift()를 수행해 정상적으로 초기화했는지 여부
	bool bGameLiftInitialized = false;

	// 현재 서버 프로세스가 AnywhereFleet인지 Managed EC2인지
	bool bIsAnywhereActive = false;

	TSharedPtr<FProcessParameters> ProcessParameters;

	void SetServerParameters(FServerParameters& OutServerParameters);
	static void ParseCommandLinePort(int32& OutPort);

	// ============================================================================
	// GameSessions
	// ============================================================================
	// 플레이어가 수행하는 Aws GameSessions API 관련 기능
	// 서버 접속 전 Standalone, 서버 접속 후 Client
public:
	void CreateGameSession(const FString& RoomName, const FString& MatchMode, const FString& MaxPlayers, const FString& CreatorId, const FString& GoalKillCount);
	void DescribeGameSessionStatus(const FString& GameSessionId);
	void DescribeGameSessions();
	void CreatePlayerSession(const FString& Username, const FString& GameSessionId);
	void TerminateGameSession(const FString& GameSessionId);

	FOnDescribeGameSessionsSucceededDelegate OnDescribeGameSessionsSucceededDelegate;

	// URL String을 브로드캐스트한다.
	FOnAPIRequestSucceededWithOneStringDelegate OnCreatePlayerSessionSucceededDelegate;

	FAPIStatusMessageDelegate CreateGameSessionStatusMessageDelegate;

	FAPIStatusMessageDelegate DescribeGameSessionsStatusMessageDelegate;

	FAPIStatusMessageDelegate CreatePlayerSessionStatusMessageDelegate;

private:
	UPROPERTY(EditDefaultsOnly, Category = "GameSessions")
	TObjectPtr<UAPIData> GameSessionsAPIData;

	void CreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void DescribeGameSessionStatus_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 최근에 생성한 GameSession 정보
	FGameSessionInfo CreatedGameSession;

	// 생성한 GameSession의 상태가 ACTIVE인지 체크하는 타이머를 등록한다.
	// 최대 MaxCreatedGameSessionStatusPollAttempts 만큼 반복하고, 체크 간격은 CreatedGameSessionStatusPollInterval 시간이다.
	// ACTIVE 상태가 되면 CreatePlayerSession()을 수행한다.
	void PollCreatedGameSessionStatus();
	FTimerHandle DescribeGameSessionStatusPollTimer;

	// 생성한 GameSession의 상태가 ACTIVE 인지 체크하는 시간 간격 
	UPROPERTY(EditDefaultsOnly, Category = "GameSessions")
	float CreatedGameSessionStatusPollInterval;

	// 생성한 GameSession의 상태가 ACTIVE 인지 체크하는 최대 시도 횟수
	UPROPERTY(EditDefaultsOnly, Category = "GameSessions")
	int32 MaxCreatedGameSessionStatusPollAttempts;

	// 현재까지 체크를 시도한 횟수
	int32 CreatedGameSessionStatusPollAttemptCount = 0;
	
	void DescribeGameSessions_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	void TerminateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString GameSessionId);

	// TerminateGameSession 재시도를 위한 타이머 핸들
	FTimerHandle TerminateGameSessionPollTimer;

	// 재시도할 GameSession ID
	FString SessionIdToTerminate;

	// TerminateGameSession 재시도 간격 (초)
	UPROPERTY(EditDefaultsOnly, Category = "GameSessions")
	float TerminateGameSessionPollInterval;
	
	// TerminateGameSession 최대 재시도 횟수
	UPROPERTY(EditDefaultsOnly, Category = "GameSessions")
	int32 MaxTerminateGameSessionPollAttempts;

	// 현재까지의 재시도 횟수
	int32 TerminateGameSessionPollCount = 0;

	// TerminateGameSession 재시도
	void PollTerminateGameSession();
};
