// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_ToggleCamera.h"

#include "FPGameplayTags.h"
#include "Character/FPCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_ToggleCamera)

UFPGameplayAbility_ToggleCamera::UFPGameplayAbility_ToggleCamera()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	AbilityTags.AddTag(FPGameplayTags::Ability::ToggleCamera);
}

void UFPGameplayAbility_ToggleCamera::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Local Only
	if (AFPCharacter* Character = Cast<AFPCharacter>(ActorInfo->AvatarActor))
	{
		Character->ToggleCamera();
	}
	
	K2_EndAbilityLocally();
}
