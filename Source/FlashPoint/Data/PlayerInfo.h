
#pragma once

#include "MatchTypes.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "PlayerInfo.generated.h"

/**
 * 서버에 접속한 플레이어 정보
 */
USTRUCT()
struct FPlayerInfo : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend struct FPlayerInfoArray;

public:
	FPlayerInfo() { }
	FPlayerInfo(const FString& InUsername, ETeam InTeam)
		: Team(InTeam)
	{
		SetUsername(InUsername);
	}

	void SetUsername(const FString& InUsername);
	FString GetUsername() const { return Username; }
	FName GetUsernameAsFName() const { return Username_FName; }

	UPROPERTY()
	ETeam Team{};

	UPROPERTY()
	int32 KillCount{};

	UPROPERTY()
	int32 DeathCount{};

	bool operator==(const FPlayerInfo& Other) const
	{
		return Username == Other.Username;
	}

private:
	UPROPERTY()
	FString Username;

	// FName 타입의 Username 데이터
	// Username이 설정될 때 함께 설정된다.
	UPROPERTY()
	FName Username_FName;
};

/**
 * 서버에 접속한 플레이어 정보를 배열로 저장하고, Fast TArray Replication을 사용하는 구조체
 */
USTRUCT()
struct FPlayerInfoArray : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	const TArray<FPlayerInfo>& GetPlayers() const { return Players; }
	
	void AddPlayer(const FPlayerInfo& InPlayerInfo);
	void RemovePlayer(const FString& Username);
	void UpdatePlayer(const FPlayerInfo& InPlayerInfo);

	//~FFastArraySerializer contract
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FPlayerInfo, FPlayerInfoArray>(Players, DeltaParams, *this);
	}

	TArray<FPlayerInfo>& GetAddedPlayerInfos() { return AddedPlayerInfos; }
	TArray<FPlayerInfo>& GetRemovedPlayerInfos() { return RemovedPlayerInfos; }
	TArray<FPlayerInfo>& GetChangedPlayerInfos() { return ChangedPlayerInfos; }

private:
	UPROPERTY()
	TArray<FPlayerInfo> Players;

	// 클라이언트에서의 추가된 Delta Data
	UPROPERTY(NotReplicated)
	TArray<FPlayerInfo> AddedPlayerInfos;

	// 클라이언트에서의 제거된 Delta Data
	UPROPERTY(NotReplicated)
	TArray<FPlayerInfo> RemovedPlayerInfos;

	// 클라이언트에서의 변경된 Delta Data
	UPROPERTY(NotReplicated)
	TArray<FPlayerInfo> ChangedPlayerInfos;
};

template<>
struct TStructOpsTypeTraits<FPlayerInfoArray> : public TStructOpsTypeTraitsBase2<FPlayerInfoArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
