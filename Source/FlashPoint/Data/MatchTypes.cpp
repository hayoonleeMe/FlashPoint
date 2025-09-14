
#include "MatchTypes.h"

#include "FPLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MatchTypes)

FMatchInfo::FMatchInfo(const FGameSessionInfo& GameSessionInfo)
{
	GameSessionId = GameSessionInfo.GameSessionId;
	HostId = GameSessionInfo.CreatorId;
	CurrentPlayers = GameSessionInfo.CurrentPlayerSessionCount;
	RetrieveGameProperties(GameSessionInfo.GameProperties);
}

void FMatchInfo::RetrieveGameProperties(const TArray<FKeyValueStruct>& GameProperties)
{
	for (const FKeyValueStruct& Property : GameProperties)
	{
		if (Property.Key == TEXT("roomName"))
		{
			RoomName = Property.Value;
		}
		else if (Property.Key == TEXT("matchMode"))
		{
			MatchMode = StringToLex(Property.Value);
		}
		else if (Property.Key == TEXT("maxPlayers"))
		{
			MaxPlayers = Property.Value;
		}
	}
}

void FMatchInfo::Dump() const
{
	UE_LOG(LogFP, Log, TEXT("GameSessionId %s"), *GameSessionId);
	UE_LOG(LogFP, Log, TEXT("  HostId %s"), *HostId);
	UE_LOG(LogFP, Log, TEXT("  RoomName %s"), *RoomName);
	UE_LOG(LogFP, Log, TEXT("  MatchMode %s"), *LexToString(MatchMode));
	UE_LOG(LogFP, Log, TEXT("  MaxPlayers %s"), *MaxPlayers);
	UE_LOG(LogFP, Log, TEXT("  CurrentPlayers %d"), CurrentPlayers);
}
