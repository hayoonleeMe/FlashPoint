
#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagStackContainer.generated.h"

// TagStack 변경을 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTagStackChangedDelegate, const FGameplayTag&/*Tag*/, int32/*StackCount*/);

/**
 * Gameplay Tag와 해당 태그의 스택 수를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FGameplayTagStack()
		: StackCount(0)
	{}
	
	FGameplayTagStack(const FGameplayTag& InTag, int32 InStackCount)
		: Tag(InTag), StackCount(InStackCount)
	{}

private:
	friend struct FGameplayTagStackContainer;
	
	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount;
};

/**
 * 여러 Gameplay Tag와 각 태그의 Stack 수를 관리하는 컨테이너.
 * FFastArraySerializer를 상속받아, 태그가 추가, 제거되거나 스택 수가 변경될 때
 * 전체 목록이 아닌 변경 사항만 Replicate해 매우 효율적인 네트워크 동기화를 지원한다.
 */
USTRUCT(BlueprintType)
struct FGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	// 컨테이너에 TagStack을 추가하거나, 이미 있다면 StackCount를 업데이트한다.
	// InStackCount가 0보다 작으면 수행하지 않는다.
	void AddTagStack(const FGameplayTag& InTag, int32 InStackCount);

	// 컨테이너에서 InTag에 해당하는 TagStack을 제거한다.
	void RemoveTagStack(const FGameplayTag& InTag);

	// 컨테이너에 InTag에 해당하는 TagStack이 있는지 여부를 반환한다.
	bool ContainsTag(const FGameplayTag& InTag) const;

	// InTag에 해당하는 StackCount를 반환한다.
	// 컨테이너에 존재하지 않으면 0을 반환한다.
	int32 GetStackCount(const FGameplayTag& InTag) const;

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	FOnTagStackChangedDelegate OnTagStackChangedDelegate;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(TagStacks, DeltaParms, *this);
	}

private:
	UPROPERTY()
	TArray<FGameplayTagStack> TagStacks;

	// 빠른 쿼리를 위한 맵
	// 수동으로 클라이언트에서도 서버와 동기화된다.
	TMap<FGameplayTag, int32> TagStackQueryMap;
};

template<>
struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};
