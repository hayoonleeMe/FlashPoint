#pragma once

#include "OnlineServiceTypes.generated.h"

// TODO : Localization with String Table

namespace OnlineServiceStatusMessage
{
	constexpr const TCHAR* SomethingWentWrong = TEXT("Something went wrong!");

	namespace SignIn
	{
		constexpr const TCHAR* SignInPending = TEXT("Signing in...");
		constexpr const TCHAR* IncorrectCredentials = TEXT("Incorrect username or password.");
		constexpr const TCHAR* UserNotFound = TEXT("User not found.");
	}
	
	namespace SignUp
	{
		constexpr const TCHAR* SignUpPending = TEXT("Creating a new account...");
		constexpr const TCHAR* InvalidUsername = TEXT("Please enter a valid Username.");
		constexpr const TCHAR* PasswordTooShort = TEXT("Passwords must be at least 8 characters.");
		constexpr const TCHAR* PasswordMissingNumber = TEXT("Password must contain at least one number.");
		constexpr const TCHAR* PasswordMissingSpecialChar = TEXT("Password must contain at least one special character.");
		constexpr const TCHAR* PasswordMissingUppercase = TEXT("Password must contain at least one uppercase character.");
		constexpr const TCHAR* PasswordMissingLowercase = TEXT("Password must contain at least one lowercase character.");
		constexpr const TCHAR* PasswordsMismatch = TEXT("Please ensure that passwords match.");
		constexpr const TCHAR* InvalidEmail = TEXT("Please enter a valid email.");
		constexpr const TCHAR* UsernameExists = TEXT("Username already exists.");
		constexpr const TCHAR* EmailAlreadyInUse = TEXT("An account with this email already exists.");
		constexpr const TCHAR* UserAlreadyRegistered = TEXT("This user is already registered.");
	}

	namespace ConfirmSignUp
	{
		constexpr const TCHAR* CheckConfirmCodePending = TEXT("Checking confirmation code...");
		constexpr const TCHAR* IncorrectConfirmationCode = TEXT("Incorrect confirmation code.");
		constexpr const TCHAR* InvalidConfirmationCode = TEXT("Please enter six numerical digits.");
		constexpr const TCHAR* ConfirmationCodeExpired = TEXT("The confirmation code has expired.");
	}

	namespace CreateGameSession
	{
		constexpr const TCHAR* CreateMatchPending = TEXT("Creating match...");
		constexpr const TCHAR* InvalidRoomName = TEXT("Please enter a valid room name.");
		constexpr const TCHAR* ServerCapacityFull = TEXT("Cannot create a match.\nAll servers are currently busy.");
		constexpr const TCHAR* GameSessionStartFailed = TEXT("Failed to start the game session due to a server error.");
	}

	namespace DescribeGameSessions
	{
		constexpr const TCHAR* FindMatchPending = TEXT("Searching for matches...");
		constexpr const TCHAR* FindMatchesFailed = TEXT("Failed to find matches.");
	}

	namespace CreatePlayerSession
	{
		constexpr const TCHAR* JoinMatchPending = TEXT("Joining match...");
		constexpr const TCHAR* MatchIsFull = TEXT("This match is full.");
	}
}

USTRUCT()
struct FHTTPMetaData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 httpStatusCode{};

	UPROPERTY()
	FString requestId;

	UPROPERTY()
	int32 attempts{};

	UPROPERTY()
	double totalRetryDelay{};

	void Dump() const;
};

// ============================================================================
// PlayerSign
// ============================================================================

USTRUCT()
struct FCodeDeliveryDetails
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString AttributeName;
	
	UPROPERTY()
	FString DeliveryMedium;
	
	UPROPERTY()
	FString Destination;
	
	void Dump() const;
};

USTRUCT()
struct FSignUpResponse
{
	GENERATED_BODY()
	
	UPROPERTY()
	FCodeDeliveryDetails CodeDeliveryDetails;
	
	UPROPERTY()
	bool UserConfirmed{};
	
	UPROPERTY()
	FString UserSub;
	
	void Dump() const;
};

USTRUCT()
struct FNewDeviceMetaData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString DeviceGroupKey;
	
	UPROPERTY()
	FString DeviceKey;
	
	void Dump() const;
};

USTRUCT()
struct FAuthenticationResult
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString AccessToken;
	
	UPROPERTY()
	int32 ExpiresIn{};
	
	UPROPERTY()
	FString IdToken;
	
	UPROPERTY()
	FNewDeviceMetaData NewDeviceMetadata;
	
	UPROPERTY()
	FString RefreshToken;
	
	UPROPERTY()
	FString TokenType;
	
	void Dump() const;
};

USTRUCT()
struct FChallengeParameters
{
	GENERATED_BODY()
	
	UPROPERTY()
	TMap<FString, FString> Parameters;
	
	void Dump() const;
};

USTRUCT()
struct FInitiateAuthResponse
{
	GENERATED_BODY()
	
	UPROPERTY()
	FAuthenticationResult AuthenticationResult;
	
	UPROPERTY()
	FString ChallengeName;
	
	UPROPERTY()
	FChallengeParameters ChallengeParameters;
	
	UPROPERTY()
	FString Session;

	UPROPERTY()
	FString Email;
	
	void Dump() const;
};

// ============================================================================
// GameSessions
// ============================================================================

USTRUCT()
struct FKeyValueStruct
{
	GENERATED_BODY()

	UPROPERTY()
	FString Key;

	UPROPERTY()
	FString Value;
};

/**
 * 검색된 단일 게임 세션의 정보를 담는 구조체
 */
USTRUCT()
struct FGameSessionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString CreationTime;

	UPROPERTY()
	FString CreatorId;

	UPROPERTY()
	int32 CurrentPlayerSessionCount{};

	UPROPERTY()
	FString DnsName;

	UPROPERTY()
	FString FleetArn;

	UPROPERTY()
	FString FleetId;

	UPROPERTY()
	TArray<FKeyValueStruct> GameProperties;

	UPROPERTY()
	FString GameSessionData;

	UPROPERTY()
	FString GameSessionId;
	
	UPROPERTY()
	FString IpAddress;

	UPROPERTY()
	FString Location;

	UPROPERTY()
	FString MatchmakerData;

	UPROPERTY()
	int32 MaximumPlayerSessionCount{};

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FString PlayerSessionCreationPolicy;

	UPROPERTY()
	int32 Port{};

	UPROPERTY()
	FString Status;
	
	UPROPERTY()
	FString StatusReason;
	
	UPROPERTY()
	FString TerminationTime;

	void Dump() const;
};

USTRUCT()
struct FDescribeGameSessionsResponse
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGameSessionInfo> GameSessions;

	void Dump() const;
};

USTRUCT()
struct FPlayerSessionInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString CreationTime;
	
	UPROPERTY()
	FString DnsName;
	
	UPROPERTY()
	FString FleetArn;
	
	UPROPERTY()
	FString FleetId;
	
	UPROPERTY()
	FString GameSessionId;
	
	UPROPERTY()
	FString IpAddress;
	
	UPROPERTY()
	FString PlayerData;
	
	UPROPERTY()
	FString PlayerId;
	
	UPROPERTY()
	FString PlayerSessionId;
	
	UPROPERTY()
	int32 Port{};
	
	UPROPERTY()
	FString Status;
	
	UPROPERTY()
	FString TerminationTime;
	
	void Dump() const;
};
