
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
