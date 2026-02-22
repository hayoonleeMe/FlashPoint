// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_AimDownSight.h"

#include "AbilitySystemComponent.h"
#include "FPGameplayTags.h"
#include "Character/FPCharacter.h"
#include "Component/UIManageComponent.h"
#include "UI/Gameplay/PlayerHUD.h"
#include "Weapon/WeaponManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_AimDownSight)

UFPGameplayAbility_AimDownSight::UFPGameplayAbility_AimDownSight()
{
	AbilityTags.AddTag(FPGameplayTags::Ability::AimDownSight);
	ActivationOwnedTags.AddTag(FPGameplayTags::CharacterState::IsAimingDownSight);
	ActivationBlockedTags.AddTag(FPGameplayTags::Weapon::NoFire);
	CancelAbilitiesWithTag.AddTag(FPGameplayTags::Ability::Sprint);
	ActivationRequiredTags.AddTag(FPGameplayTags::CharacterState::IsEquippingWeapon);
}

void UFPGameplayAbility_AimDownSight::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_EndAbility();
}

bool UFPGameplayAbility_AimDownSight::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// 이전 ADS가 끝나 다시 시작할 수 있는지 체크
	const AFPCharacter* FPCharacter = Cast<AFPCharacter>(ActorInfo->AvatarActor);
	if (!IsValid(FPCharacter) || !FPCharacter->CanStartAimDownSight())
	{
		return false;
	}
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UFPGameplayAbility_AimDownSight::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 카메라 전환, 이동속도 변경 요청 등은 로컬에서만 수행
	AFPCharacter* FPCharacter = Cast<AFPCharacter>(ActorInfo->AvatarActor);
	if (FPCharacter && FPCharacter->IsLocallyControlled())
	{
		bIsThirdPersonBeforeADS = !GetAbilitySystemComponentFromActorInfo_Checked()->HasMatchingGameplayTag(FPGameplayTags::CharacterState::IsFirstPerson);
		if (bIsThirdPersonBeforeADS)
		{
			FPCharacter->ToggleCamera();
		}
		
		FPCharacter->StartAimDownSight();
		
		// Hide Crosshair
		if (const UUIManageComponent* UIManageComponent = UUIManageComponent::Get(FPCharacter->GetController<APlayerController>()))
		{
			if (const UPlayerHUD* PlayerHUDWidget = UIManageComponent->GetMainHUDWidget<UPlayerHUD>())
			{
				PlayerHUDWidget->ShowCrosshair(false);
			}
		}
	}
}

void UFPGameplayAbility_AimDownSight::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AFPCharacter* FPCharacter = Cast<AFPCharacter>(ActorInfo->AvatarActor);
	if (FPCharacter && FPCharacter->IsLocallyControlled())
	{
		if (bIsThirdPersonBeforeADS)
		{
			FPCharacter->ToggleCamera();
		}
		
		FPCharacter->StopAimDownSight();
		
		// Show Crosshair
		if (const UUIManageComponent* UIManageComponent = UUIManageComponent::Get(FPCharacter->GetController<APlayerController>()))
		{
			if (const UPlayerHUD* PlayerHUDWidget = UIManageComponent->GetMainHUDWidget<UPlayerHUD>())
			{
				PlayerHUDWidget->ShowCrosshair(true);
			}
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
