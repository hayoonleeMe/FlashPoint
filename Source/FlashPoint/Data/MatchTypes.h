
#pragma once

#include "Data/OnlineServiceTypes.h"

#include "MatchTypes.generated.h"

constexpr const TCHAR* KickReasonOption = TEXT("KickReason");
constexpr const TCHAR* HostLogoutKickReason = TEXT("The host has left the match.");
constexpr const TCHAR* SessionTerminatedKickReason = TEXT("The game session has terminated.");

/**
 * 현재 매치의 모드를 나타내는 enum class
 */
UENUM()
enum class EMatchMode : uint8
{
	None,
	TeamDeathMatch,
	FreeForAll
};

// EMatchMode 값을 FString으로 변환한다.
static FString LexToString(EMatchMode MatchMode)
{
	switch (MatchMode)
	{
	case EMatchMode::TeamDeathMatch:
		return TEXT("Team Death Match");
		
	case EMatchMode::FreeForAll:
		return TEXT("Free For All");
		
	default:
		return TEXT("None");
	}
}

// FString 값을 EMatchMode 값으로 변환한다.
static EMatchMode StringToLex(const FString& MatchModeStr)
{
	if (MatchModeStr == LexToString(EMatchMode::TeamDeathMatch))
	{
		return EMatchMode::TeamDeathMatch;
	}
	if (MatchModeStr == LexToString(EMatchMode::FreeForAll))
	{
		return EMatchMode::FreeForAll;
	}
	return EMatchMode::None;
}

/**
 * 현재 매치 정보를 저장하는 구조체
 */
USTRUCT()
struct FMatchInfo
{
	GENERATED_BODY()

	FMatchInfo()
	{ }

	FMatchInfo(const FGameSessionInfo& GameSessionInfo);

	UPROPERTY()
	FString GameSessionId;

	UPROPERTY()
	FString HostId;

	UPROPERTY()
	FString RoomName;

	UPROPERTY()
	EMatchMode MatchMode{};

	UPROPERTY()
	FString MaxPlayers;

	UPROPERTY()
	int32 CurrentPlayers{};

	UPROPERTY()
	int32 GoalKillCount{};

	// TArray<FKeyValueStruct> 타입의 데이터를 변환해 내부 데이터를 설정한다.
	void RetrieveGameProperties(const TArray<FKeyValueStruct>& GameProperties);
	void Dump() const;
};

/**
 * Team Death Match의 팀을 나타내는 enum class
 */
UENUM()
enum class ETeam
{
	RedTeam,
	BlueTeam,
	None
};

static FString LexToString(ETeam Team)
{
	switch (Team)
	{
	case ETeam::RedTeam:
		return TEXT("Red Team");

	case ETeam::BlueTeam:
		return TEXT("Blue Team");

	default:
		return TEXT("None");
	}
}

// Rank에 해당되는 서수 문자열을 반환한다.
static FString GetOrdinalString(int32 Rank)
{
	if (Rank <= 0)
	{
		return FString();
	}

	// 마지막 두 자리 숫자가 11, 12, 13인 경우는 항상 'th'를 사용한다.
	const int32 LastTwoDigits = Rank % 100;
	if (LastTwoDigits >= 11 && LastTwoDigits <= 13)
	{
		return FString::Printf(TEXT("%dth"), Rank);
	}

	// 그 외의 경우, 마지막 한 자리 숫자에 따라 접미사를 결정한다.
	switch (Rank % 10)
	{
	case 1:
		return FString::Printf(TEXT("%dst"), Rank);
	case 2:
		return FString::Printf(TEXT("%dnd"), Rank);
	case 3:
		return FString::Printf(TEXT("%drd"), Rank);
	default:
		return FString::Printf(TEXT("%dth"), Rank);
	}
}
