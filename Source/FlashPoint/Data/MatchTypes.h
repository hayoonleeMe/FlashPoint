
#pragma once

#include "Data/OnlineServiceTypes.h"

#include "MatchTypes.generated.h"

/**
 * 현재 매치의 모드를 나타내는 enum class
 */
UENUM()
enum class EMatchMode : uint8
{
	TeamDeathMatch,
	FreeForAll,
	None
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
	FString CurrentPlayers;

	// TArray<FKeyValueStruct> 타입의 데이터를 변환해 내부 데이터를 설정한다.
	void RetrieveGameProperties(const TArray<FKeyValueStruct>& GameProperties);
	void Dump() const;
};
