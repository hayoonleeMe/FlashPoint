
#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "FPGameplayAbilityTargetData_SingleTargetHit.generated.h"

/**
 * 추가 데이터를 저장하는 FGameplayAbilityTargetData_SingleTargetHit의 하위 구조체
 */
USTRUCT()
struct FFPGameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()

	FFPGameplayAbilityTargetData_SingleTargetHit()
	{ }

	FFPGameplayAbilityTargetData_SingleTargetHit(const FHitResult& InHitResult)
		: FGameplayAbilityTargetData_SingleTargetHit(InHitResult)
	{ }

	UPROPERTY()
	bool bCanApplyDamage = false;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FFPGameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}
};

template<>
struct TStructOpsTypeTraits<FFPGameplayAbilityTargetData_SingleTargetHit> : public TStructOpsTypeTraitsBase2<FFPGameplayAbilityTargetData_SingleTargetHit>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
