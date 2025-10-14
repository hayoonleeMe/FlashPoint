// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_Reload.h"

#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_Reload)

UFPGameplayAbility_Reload::UFPGameplayAbility_Reload()
{
	AbilityTags.AddTag(FPGameplayTags::Ability::Reload);
	ActivationOwnedTags.AddTag(FPGameplayTags::Weapon::NoFire);
	CancelAbilitiesWithTag.AddTag(FPGameplayTags::Ability::WeaponFire);

	PlayRate = 1.f;
}

bool UFPGameplayAbility_Reload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                   const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// ActorInfo is already valid (checked in Super::CanActivateAbility)
	// 장착 중인 무기가 유효한지 체크
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	AWeapon_Base* Weapon = GetEquippedWeapon(AvatarActor);
	if (!IsValid(Weapon))
	{
		UE_LOG(LogFP, Warning, TEXT("[%hs] Can't activate ability because of invalid equipped weapon."), __FUNCTION__);
		return false;
	}

	if (UWeaponManageComponent* WeaponManageComponent = AvatarActor->FindComponentByClass<UWeaponManageComponent>())
	{
		// 최소한의 ReserveAmmo가 있는지 체크
		if (WeaponManageComponent->GetAmmoTagStacks().GetStackCount(Weapon->GetWeaponTypeTag()) < 1)
		{
			return false;
		}

		// 이미 탄창이 가득찼는지 체크
		if (WeaponManageComponent->GetAmmoTagStacks().GetStackCount(FPGameplayTags::Weapon::Data::Ammo) == Weapon->GetMagCapacity())
		{
			return false;
		}
	}

	return true;
}

void UFPGameplayAbility_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CharacterReloadMontage)
	{
		// Play Character Reload Anim Montage
		if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Play Character Reload Montage"), CharacterReloadMontage, PlayRate))
		{
			// Complete될 때는 로컬에서만 종료(서버에서 계속 수행하도록)
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbilityLocally);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->ReadyForActivation();
		}
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FPGameplayTags::GameplayEvent::ReloadFinished, nullptr, true, true))
		{
			// Ammo를 변경하는 실제 재장전은 서버에서만 수행
			WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnReloadFinished);
			WaitEventTask->ReadyForActivation();
		}
	}
}

AWeapon_Base* UFPGameplayAbility_Reload::GetEquippedWeapon(const AActor* AvatarActor) const
{
	if (AvatarActor)
	{
		if (UWeaponManageComponent* WeaponManageComp = AvatarActor->FindComponentByClass<UWeaponManageComponent>())
		{
			return WeaponManageComp->GetEquippedWeapon();
		}
	}
	return nullptr;
}

void UFPGameplayAbility_Reload::OnReloadFinished(FGameplayEventData Payload)
{
	Reload();
	K2_EndAbility();
}

void UFPGameplayAbility_Reload::Reload() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	check(AvatarActor);
	
	if (AWeapon_Base* Weapon = GetEquippedWeapon(AvatarActor))
	{
		if (UWeaponManageComponent* WeaponManageComponent = AvatarActor->FindComponentByClass<UWeaponManageComponent>())
		{
			FGameplayTagStackContainer& AmmoTagStacks = WeaponManageComponent->GetAmmoTagStacks();
			int32 Ammo = AmmoTagStacks.GetStackCount(FPGameplayTags::Weapon::Data::Ammo);
			int32 ReserveAmmo = AmmoTagStacks.GetStackCount(Weapon->GetWeaponTypeTag());
			const int32 MagCapacity = Weapon->GetMagCapacity();

			int32 Amount = FMath::Min(MagCapacity - Ammo, ReserveAmmo);
			Ammo += Amount;
			ReserveAmmo -= Amount;

			AmmoTagStacks.AddTagStack(FPGameplayTags::Weapon::Data::Ammo, Ammo);
			AmmoTagStacks.AddTagStack(Weapon->GetWeaponTypeTag(), ReserveAmmo);
		}
	}
}
