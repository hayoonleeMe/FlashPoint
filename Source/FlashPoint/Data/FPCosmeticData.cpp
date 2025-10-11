// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCosmeticData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCosmeticData)

TSubclassOf<UAnimInstance> UFPCosmeticData::SelectAnimLayer(const FGameplayTagContainer& CosmeticTags) const
{
	for (const FAnimLayerEntry& Entry : AnimLayerSet.LayerEntries)
	{
		if (Entry.AnimLayer != nullptr && CosmeticTags.HasAll(Entry.RequiredTags))
		{
			return Entry.AnimLayer;
		}
	}

	return AnimLayerSet.DefaultAnimLayer;
}

TSubclassOf<UAnimInstance> UFPCosmeticData::GetDefaultAnimLayer() const
{
	return AnimLayerSet.DefaultAnimLayer;
}

USkeletalMesh* UFPCosmeticData::GetCharacterMesh() const
{
	return CharacterMeshSet.CharacterMesh;
}

const FCharacterMeshEntry* UFPCosmeticData::GetCharacterMeshEntryByTeam(ETeam Team) const
{
	return CharacterMeshSet.TeamMeshMap.Find(Team);
}
