
#include "PlayerInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerInfo)

void FPlayerInfoArray::AddPlayer(const FString& Username, ETeam Team)
{
	FPlayerInfo& PlayerInfo = Players.AddDefaulted_GetRef();
	PlayerInfo.Username = Username;
	PlayerInfo.Team = Team;
	MarkItemDirty(PlayerInfo);
}

void FPlayerInfoArray::RemovePlayer(const FString& Username)
{
	for (auto It = Players.CreateIterator(); It; ++It)
	{
		FPlayerInfo& PlayerInfo = *It;
		if (PlayerInfo.Username == Username)
		{
			It.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FPlayerInfoArray::UpdatePlayer(const FString& Username, ETeam Team)
{
	for (auto It = Players.CreateIterator(); It; ++It)
	{
		FPlayerInfo& PlayerInfo = *It;
		if (PlayerInfo.Username == Username)
		{
			PlayerInfo.Team = Team;
			MarkItemDirty(PlayerInfo);
			return;
		}
	}
}

void FPlayerInfoArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	AddedPlayerInfos.Reset();
	for (int32 AddedIndex : AddedIndices)
	{
		AddedPlayerInfos.Add(Players[AddedIndex]);
	}
}

void FPlayerInfoArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	RemovedPlayerInfos.Reset();
	for (int32 RemovedIndex : RemovedIndices)
	{
		RemovedPlayerInfos.Add(Players[RemovedIndex]);
	}
}

void FPlayerInfoArray::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	ChangedPlayerInfos.Reset();
	for (int32 ChangedIndex : ChangedIndices)
	{
		ChangedPlayerInfos.Add(Players[ChangedIndex]);
	}
}
