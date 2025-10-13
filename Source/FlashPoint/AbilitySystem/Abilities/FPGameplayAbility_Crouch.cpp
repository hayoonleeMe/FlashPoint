// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_Crouch.h"

#include "FPGameplayTags.h"
#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UFPGameplayAbility_Crouch::UFPGameplayAbility_Crouch()
{
	// ACharacter::Crouch 동작 자체가 알아서 모든 기기에 반영됨
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	AbilityTags.AddTag(FPGameplayTags::Ability::Crouch);
	CancelAbilitiesWithTag.AddTag(FPGameplayTags::Ability::Sprint);
}

void UFPGameplayAbility_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Crouch
	if (ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComponent = AvatarCharacter->GetCharacterMovement())
		{
			if (MovementComponent->IsMovingOnGround())
			{
				if (AvatarCharacter->bIsCrouched)
				{
					AvatarCharacter->UnCrouch();
				}
				else
				{
					AvatarCharacter->Crouch();
				}
			}
		}
	}

	// Flush Sprint Input
	if (UFPAbilitySystemComponent* ASC = Cast<UFPAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		ASC->FlushPressedInput(FPGameplayTags::Input::Gameplay::Sprint);
	}

	K2_EndAbility();
}
