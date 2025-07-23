
#include "GameplayTagStackContainer.h"

void FGameplayTagStackContainer::AddTagStack(const FGameplayTag& InTag, int32 InStackCount)
{
	if (InStackCount < 0)
	{
		return;
	}
	
	for (FGameplayTagStack& Entry : TagStacks)
	{
		if (Entry.Tag.MatchesTagExact(InTag) && Entry.StackCount != InStackCount)
		{
			// 이미 존재하면 StackCount 업데이트 (StackCount 다를 때만)
			Entry.StackCount = InStackCount;
			TagStackQueryMap[InTag] = InStackCount;
			MarkItemDirty(Entry);
			return;
		}
	}

	// 없으면 새로 추가
	FGameplayTagStack& NewStack = TagStacks.Emplace_GetRef(InTag, InStackCount);
	TagStackQueryMap.Add(InTag, InStackCount);
	MarkItemDirty(NewStack);
}

void FGameplayTagStackContainer::RemoveTagStack(const FGameplayTag& InTag)
{
	for (auto It = TagStacks.CreateIterator(); It; ++It)
	{
		if (It->Tag.MatchesTagExact(InTag))
		{
			It.RemoveCurrent();
			TagStackQueryMap.Remove(InTag);
			MarkArrayDirty();
			return;
		}
	}
}

bool FGameplayTagStackContainer::ContainsTag(const FGameplayTag& InTag) const
{
	return TagStackQueryMap.Contains(InTag);
}

int32 FGameplayTagStackContainer::GetStackCount(const FGameplayTag& InTag) const
{
	return TagStackQueryMap.FindRef(InTag);
}

void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		TagStackQueryMap.Remove(TagStacks[Index].Tag);
	}
}

void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FGameplayTagStack& TagStack = TagStacks[Index];
		TagStackQueryMap.Add(TagStack.Tag, TagStack.StackCount);
	}
}

void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FGameplayTagStack& TagStack = TagStacks[Index];
		TagStackQueryMap[TagStack.Tag] = TagStack.StackCount;
	}
}
