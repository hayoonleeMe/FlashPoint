// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_Reload.h"

#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "Abilities/Tasks/AbilityTask_PlayAnimAndWait.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Component/WeaponWallTraceComponent.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_Reload)

UFPGameplayAbility_Reload::UFPGameplayAbility_Reload()
{
	AbilityTags.AddTag(FPGameplayTags::Ability::Reload);
	ActivationOwnedTags.AddTag(FPGameplayTags::Weapon::NoFire);
	ActivationOwnedTags.AddTag(FPGameplayTags::CharacterState::IsReloading);
	CancelAbilitiesWithTag.AddTag(FPGameplayTags::Ability::WeaponFire);

	PlayRate = 1.f;
	MagazineAttachBoneName = TEXT("hand_l");
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

	bool bIsWeaponWallBlocked = false, bUseWeaponUp = false;
	if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		if (UWeaponWallTraceComponent* WeaponWallTraceComponent = AvatarActor->FindComponentByClass<UWeaponWallTraceComponent>())
		{
			// Register Delegate
			WeaponWallBlockDataChangedDelegateHandle = WeaponWallTraceComponent->WeaponWallBlockDataChangedDelegate.AddUObject(this, &ThisClass::OnWeaponWallBlockDataChanged);
			WeaponWallTraceComponent->RetrieveWeaponWallBlockData(bIsWeaponWallBlocked, bUseWeaponUp);
		}
	}

	PlayReloadMontage(false, bIsWeaponWallBlocked, bUseWeaponUp);
	// if (CharacterReloadMontage)
	// {
	// 	// Play Character Reload Anim Montage
	// 	if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Play Character Reload Montage"), CharacterReloadMontage, PlayRate, InitialSectionName))
	// 	{
	// 		// Complete될 때는 로컬에서만 종료(서버에서 계속 수행하도록)
	// 		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbilityLocally);
	// 		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
	// 		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	// 		MontageTask->ReadyForActivation();
	// 	}
	// }

	// CharacterState.IsReloading 태그가 추가되면 그에 따라 ABP에서 재장전 애니메이션을 수행하고, 해당 애니메이션의 Notify에 의해 발생하는 이벤트를 대기
	if (HasAuthority(&CurrentActivationInfo))
	{
		if (UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FPGameplayTags::GameplayEvent::ReloadFinished, nullptr, true, true))
		{
			// Ammo를 변경하는 실제 재장전은 서버에서만 수행
			WaitEventTask->EventReceived.AddDynamic(this, &ThisClass::OnReloadFinished);
			WaitEventTask->ReadyForActivation();
		}
	}
	else
	{
		if (UAbilityTask_WaitGameplayEvent* MagazineOutEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FPGameplayTags::GameplayEvent::ReloadMagazineOut, nullptr, true, true))
		{
			MagazineOutEvent->EventReceived.AddDynamic(this, &ThisClass::OnMagazineOut);
			MagazineOutEvent->ReadyForActivation();
		}
		
		if (UAbilityTask_WaitGameplayEvent* MagazineInEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FPGameplayTags::GameplayEvent::ReloadMagazineIn, nullptr, true, true))
		{
			MagazineInEvent->EventReceived.AddDynamic(this, &ThisClass::OnMagazineIn);
			MagazineInEvent->ReadyForActivation();
		}
	}
}

void UFPGameplayAbility_Reload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("%hs"), __FUNCTION__);
	}

	if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		if (UWeaponWallTraceComponent* WeaponWallTraceComponent = AvatarActor->FindComponentByClass<UWeaponWallTraceComponent>())
		{
			// Deregister Delegate
			WeaponWallTraceComponent->WeaponWallBlockDataChangedDelegate.Remove(WeaponWallBlockDataChangedDelegateHandle);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// TODO : 함수명변경?
void UFPGameplayAbility_Reload::OnWeaponWallBlockDataChanged(bool bIsWeaponWallBlocked, bool bUseWeaponUp)
{
	PlayReloadMontage(true, bIsWeaponWallBlocked, bUseWeaponUp);
	
	// UAnimInstance* AnimInstance = GetCurrentActorInfo()->GetAnimInstance();
	// if (AnimInstance && CurrentMontage)
	// {
	// 	float ElapsedTimeFromSectionStart = 0.f;
	// 	const float CurrentPosition = AnimInstance->Montage_GetPosition(CurrentMontage);
	// 	const int32 CurrentSectionIndex = CurrentMontage->GetSectionIndexFromPosition(CurrentPosition);
	// 	if (CurrentSectionIndex != INDEX_NONE)
	// 	{
	// 		float CurrentSectionStartTime, Temp;
	// 		CurrentMontage->GetSectionStartAndEndTime(CurrentSectionIndex, CurrentSectionStartTime, Temp);
	// 		ElapsedTimeFromSectionStart = CurrentPosition - CurrentSectionStartTime;
	// 	}
	//
	// 	const int32 NewSectionIndex = CharacterReloadMontage->GetSectionIndex(SelectSectionName(bIsWeaponWallBlocked, bUseWeaponUp));
	// 	if (NewSectionIndex != INDEX_NONE)
	// 	{
	// 		float NewSectionStartTime, NewSectionEndTime;
	// 		CharacterReloadMontage->GetSectionStartAndEndTime(NewSectionIndex, NewSectionStartTime, NewSectionEndTime);
	// 		const float NewStartTime = FMath::Clamp(NewSectionStartTime + ElapsedTimeFromSectionStart, NewSectionStartTime, NewSectionEndTime);
	//
	// 		bMontageUpdated = true;
	// 		UE_LOG(LogTemp, Warning, TEXT("%hs, Frame %lld"), __FUNCTION__, UKismetSystemLibrary::GetFrameCount());
	// 		AnimInstance->Montage_PlayWithBlendIn(CurrentMontage, FAlphaBlendArgs(0.2f), PlayRate, EMontagePlayReturnType::MontageLength, NewStartTime);
	// 		//AnimInstance->Montage_SetPosition(CurrentMontage, NewStartTime);
	// 	}
	// }
}

FName UFPGameplayAbility_Reload::SelectSectionName(bool bIsWeaponWallBlocked, bool bUseWeaponUp) const
{
	// TODO : Parametrize
	if (bIsWeaponWallBlocked)
	{
		return bUseWeaponUp ? TEXT("WeaponBlockUp") : TEXT("WeaponBlockDown");
	}
	return TEXT("Default");
}

UAnimMontage* UFPGameplayAbility_Reload::SelectReloadMontage(bool bIsWeaponWallBlocked, bool bUseWeaponUp) const
{
	if (bIsWeaponWallBlocked)
	{
		return bUseWeaponUp ? WeaponBlockUpReloadMontage : WeaponBlockDownReloadMontage;
	}
	return ReloadMontage;
}

void UFPGameplayAbility_Reload::PlayReloadMontage(bool bIsUpdate, bool bIsWeaponWallBlocked, bool bUseWeaponUp)
{
	return;
	
	if (bIsUpdate && !CurrentMontage)
	{
		// 현재 몽타주 재생 중일 때만 몽타주 업데이트
		return;
	}

	// 몽타주 업데이트에 의해 Interrupt되어 어빌리티가 종료되는걸 방지하기 위해 상태 저장
	bMontageUpdated = bIsUpdate;
	
	// 현재 몽타주의 재생 시간부터 이어서 새 몽타주를 재생
	float PlaybackPosition = 0.f;
	
	if (CurrentMontage)
	{
		if (UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance())
		{
			PlaybackPosition = AnimInstance->Montage_GetPosition(CurrentMontage);
			AnimInstance->Montage_Stop(0.2f, CurrentMontage);
		}
	}

	if (UAnimMontage* NewMontage = SelectReloadMontage(bIsWeaponWallBlocked, bUseWeaponUp))
	{
		// 새 몽타주의 재생 시간을 넘지 않도록 Clamp
		float StartTime, EndTime;
		NewMontage->GetSectionStartAndEndTime(0, StartTime, EndTime);
		PlaybackPosition = FMath::Clamp(PlaybackPosition, StartTime, EndTime);

		if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Play Character Reload Montage"), NewMontage, PlayRate, NAME_None, true, 1.f, PlaybackPosition))
		{
			if (IsLocallyControlled())
			{
				UE_LOG(LogTemp, Warning, TEXT("%hs, Frame %lld, PlaybackPosition %f, bIsWeaponWallBlocked %d, bUseWeaponUp %d"), __FUNCTION__, GFrameCounter, PlaybackPosition, bIsWeaponWallBlocked, bUseWeaponUp);
			}
			
			// Complete될 때는 로컬에서만 종료(서버에서 계속 수행하도록)
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbilityLocally);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->ReadyForActivation();
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

void UFPGameplayAbility_Reload::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("%hs, Frame %lld, bMontageUpdated %d"), __FUNCTION__, GFrameCounter, bMontageUpdated);
	
	if (bMontageUpdated)
	{
		bMontageUpdated = false;
		return;
	}
	
	K2_EndAbility();
}

void UFPGameplayAbility_Reload::OnReloadFinished(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("%hs"), __FUNCTION__);
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

void UFPGameplayAbility_Reload::OnMagazineOut(FGameplayEventData Payload)
{
	if (const AWeapon_Base* Weapon = GetEquippedWeapon(GetAvatarActorFromActorInfo()))
	{
		if (USkeletalMeshComponent* WeaponMesh = Weapon->GetWeaponMesh())
		{
			WeaponMesh->HideBoneByName(MagazineBoneName, PBO_None);
		}

		if (CurrentActorInfo && CurrentActorInfo->SkeletalMeshComponent.IsValid())
		{
			if (UStaticMeshComponent* MagazineMesh = Weapon->GetWeaponMagazineMesh())
			{
				MagazineMesh->AttachToComponent(CurrentActorInfo->SkeletalMeshComponent.Get(), FAttachmentTransformRules::SnapToTargetIncludingScale, MagazineAttachBoneName);
				MagazineMesh->SetRelativeTransform(MagazineAttachTransform);
				MagazineMesh->SetHiddenInGame(false);
			}
		}
	}
}

void UFPGameplayAbility_Reload::OnMagazineIn(FGameplayEventData Payload)
{
	if (const AWeapon_Base* Weapon = GetEquippedWeapon(GetAvatarActorFromActorInfo()))
	{
		if (USkeletalMeshComponent* WeaponMesh = Weapon->GetWeaponMesh())
		{
			WeaponMesh->UnHideBoneByName(MagazineBoneName);

			if (CurrentActorInfo && CurrentActorInfo->SkeletalMeshComponent.IsValid())
			{
				if (UStaticMeshComponent* MagazineMesh = Weapon->GetWeaponMagazineMesh())
				{
					MagazineMesh->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);
					MagazineMesh->SetHiddenInGame(true);
				}
			}
		}
	}
}
