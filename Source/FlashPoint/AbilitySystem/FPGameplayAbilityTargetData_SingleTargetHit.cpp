
#include "FPGameplayAbilityTargetData_SingleTargetHit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbilityTargetData_SingleTargetHit)

bool FFPGameplayAbilityTargetData_SingleTargetHit::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayAbilityTargetData_SingleTargetHit::NetSerialize(Ar, Map, bOutSuccess);

	Ar << bCanApplyDamage;
	
	return true;
}
