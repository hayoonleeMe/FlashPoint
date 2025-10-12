// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_Sprint.h"

#include "FPGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UFPGameplayAbility_Sprint::UFPGameplayAbility_Sprint()
{
	AbilityTags.AddTag(FPGameplayTags::Ability::Sprint);
	ActivationOwnedTags.AddTag(FPGameplayTags::CharacterState::IsSprinting);

	// 총 발사 시 Sprint 제한
	ActivationBlockedTags.AddTag(FPGameplayTags::CharacterState::IsFiring);
	// TODO : Crouch 시 Cancel

	SpeedMultiplier = 1.5f;
}

void UFPGameplayAbility_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_EndAbility();
}

bool UFPGameplayAbility_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool Ret = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (Ret && ActorInfo)
	{
		if (ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor))
		{
			if (UCharacterMovementComponent* MovementComponent = AvatarCharacter->GetCharacterMovement())
			{
				return MovementComponent->IsMovingOnGround();
			}
		}
	}

	return Ret;
}

void UFPGameplayAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComponent = AvatarCharacter->GetCharacterMovement())
		{
			// Crouch 상태면 해제
			AvatarCharacter->UnCrouch();
			
			if (BaseWalkSpeed <= 0.f)
			{
				BaseWalkSpeed = MovementComponent->MaxWalkSpeed;
			}
			MovementComponent->MaxWalkSpeed = BaseWalkSpeed * SpeedMultiplier;
		}
	}
}

void UFPGameplayAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComponent = AvatarCharacter->GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = BaseWalkSpeed;
			BaseWalkSpeed = 0.f;
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
