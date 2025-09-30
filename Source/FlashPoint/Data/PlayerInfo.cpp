
#include "PlayerInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerInfo)

void FPlayerInfo::SetUsername(const FString& InUsername)
{
	Username = InUsername;
	Username_FName = FName(InUsername);
}

void FPlayerInfoArray::AddPlayer(const FPlayerInfo& InPlayerInfo)
{
	FPlayerInfo& NewPlayerInfo = Players.Add_GetRef(InPlayerInfo);
	MarkItemDirty(NewPlayerInfo);
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

void FPlayerInfoArray::UpdatePlayer(const FPlayerInfo& InPlayerInfo)
{
	for (auto It = Players.CreateIterator(); It; ++It)
	{
		FPlayerInfo& CurrentPlayerInfo = *It;
		if (CurrentPlayerInfo == InPlayerInfo)
		{
			// Update임을 나타내기 위해 기존 Item의 FFastArraySerializerItem 데이터를 복구
			int32 ReplicationID = CurrentPlayerInfo.ReplicationID;
			int32 ReplicationKey = CurrentPlayerInfo.ReplicationKey;
			int32 MostRecentArrayReplicationKey = CurrentPlayerInfo.MostRecentArrayReplicationKey;
			
			CurrentPlayerInfo = InPlayerInfo;
			CurrentPlayerInfo.ReplicationID = ReplicationID;
			CurrentPlayerInfo.ReplicationKey = ReplicationKey;
			CurrentPlayerInfo.MostRecentArrayReplicationKey = MostRecentArrayReplicationKey;
			
			MarkItemDirty(CurrentPlayerInfo);
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
