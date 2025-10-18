// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_DryFire.h"

#include "FPGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_DryFire)

UFPGameplayAbility_DryFire::UFPGameplayAbility_DryFire()
{
	AbilityTags.AddTag(FPGameplayTags::Ability::DryFire);
	ActivationBlockedTags.AddTag(FPGameplayTags::Weapon::NoFire);
}

void UFPGameplayAbility_DryFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// WeaponFire 키를 누르는 동안 계속해서 실행되지 않도록
	FlushPressedInput(FPGameplayTags::Input::Gameplay::WeaponFire);
	
	if (DryFireMontage)
	{
		if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Play DryFire Montage"), DryFireMontage))
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->ReadyForActivation();
		}
	}
}
