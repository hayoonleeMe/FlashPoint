// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_Sprint.h"

#include "AbilitySystemComponent.h"
#include "FPGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "Character/FPCharacterMovementComponent.h"

UFPGameplayAbility_Sprint::UFPGameplayAbility_Sprint()
{
	AbilityTags.AddTag(FPGameplayTags::Ability::Sprint);
	ActivationOwnedTags.AddTag(FPGameplayTags::CharacterState::IsSprinting);

	// 총 발사 시 Sprint 제한
	ActivationBlockedTags.AddTag(FPGameplayTags::CharacterState::IsFiring);
}

void UFPGameplayAbility_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	// Sprint 키 입력이 중지되면 어빌리티 종료
	K2_EndAbility();
}

bool UFPGameplayAbility_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent.Get());
	if (!MovementComponent || !MovementComponent->IsMovingOnGround())
	{
		return false;
	}
	
	// 플레이어가 전방 이동 방향 키를 통해 이동 중일 때만 Sprint 수행 가능
	if (ActorInfo->IsLocallyControlled())
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			return ASC->HasMatchingGameplayTag(FPGameplayTags::CharacterState::IsMovingForwardFromInput);
		}
	}

	return true;
}

void UFPGameplayAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAbilityTask_WaitGameplayTagRemoved* WaitTagRemoved = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, FPGameplayTags::CharacterState::IsMovingForwardFromInput, nullptr, true))
	{
		// CharacterState.IsMovingForwardFromInput GameplayTag가 제거되면 더 이상 캐릭터가 움직이지 않으므로 어빌리티를 종료한다.
		WaitTagRemoved->Removed.AddDynamic(this, &ThisClass::K2_EndAbility);
		WaitTagRemoved->ReadyForActivation();
	}
	
	if (IsLocallyControlled())
	{
		if (UFPCharacterMovementComponent* CharacterMovement = Cast<UFPCharacterMovementComponent>(ActorInfo->MovementComponent.Get()))
		{
			CharacterMovement->StartSprint();
		}
	}
}

void UFPGameplayAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UFPCharacterMovementComponent* CharacterMovement = Cast<UFPCharacterMovementComponent>(ActorInfo->MovementComponent.Get()))
	{
		CharacterMovement->StopSprint();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
