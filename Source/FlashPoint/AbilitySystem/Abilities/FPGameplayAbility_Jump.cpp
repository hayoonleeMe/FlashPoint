// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_Jump.h"

#include "FPGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Character/FPCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_Jump)

UFPGameplayAbility_Jump::UFPGameplayAbility_Jump()
{
	AbilityTags.AddTag(FPGameplayTags::Ability_Jump);
	
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
}

bool UFPGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AFPCharacter* FPCharacter = Cast<AFPCharacter>(ActorInfo->AvatarActor);
	if (!IsValid(FPCharacter) || !FPCharacter->CanJump())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UFPGameplayAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	StartJump();

	if (UAbilityTask_WaitInputRelease* Task = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		Task->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		Task->ReadyForActivation();
	}
}

void UFPGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	StopJump();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UFPGameplayAbility_Jump::StartJump() const
{
	if (AFPCharacter* FPCharacter = Cast<AFPCharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor : nullptr))
	{
		if (FPCharacter->IsLocallyControlled() && !FPCharacter->bPressedJump)
		{
			FPCharacter->UnCrouch();
			FPCharacter->Jump();
		}
	}
}

void UFPGameplayAbility_Jump::StopJump() const
{
	if (AFPCharacter* FPCharacter = Cast<AFPCharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor : nullptr))
	{
		if (FPCharacter->IsLocallyControlled() && FPCharacter->bPressedJump)
		{
			FPCharacter->StopJumping();
		}
	}
}

void UFPGameplayAbility_Jump::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
