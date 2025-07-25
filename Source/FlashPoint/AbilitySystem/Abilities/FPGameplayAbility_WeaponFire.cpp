// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_WeaponFire.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FlashPoint.h"
#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_WeaponFire)

UFPGameplayAbility_WeaponFire::UFPGameplayAbility_WeaponFire()
{
	AbilityTags.AddTag(FPGameplayTags::Ability_WeaponFire);
	ActivationOwnedTags.AddTag(FPGameplayTags::CharacterState_IsFiring);
	ActivationBlockedTags.AddTag(FPGameplayTags::Weapon_NoFire);
	ActivationPolicy = EAbilityActivationPolicy::WhileInputActive;
	AmmoCostTag = FPGameplayTags::Weapon_Data_Ammo;
}

bool UFPGameplayAbility_WeaponFire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// ActorInfo is already valid (checked in Super::CanActivateAbility)
	// 장착 중인 무기가 유효한지 체크
	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!IsValid(GetEquippedWeapon(AvatarActor)))
	{
		UE_LOG(LogFP, Warning, TEXT("[%hs] Can't activate ability because of invalid equipped weapon."), __FUNCTION__);
		return false;
	}

	return true;
}

void UFPGameplayAbility_WeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Checked();

	// 델레게이트 등록
	TargetDataSetDelegateHandle = ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReady);
	
	if (IsLocallyControlled())
	{
		StartTargeting();
	}

	if (CharacterFireMontage)
	{
		// Play Character Fire Anim Montage
		if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Play Character Fire Montage"), CharacterFireMontage))
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
			MontageTask->ReadyForActivation();
		}
	}

	// Start Fire Delay Timer
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(FireDelayTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::K2_EndAbility), FireDelay, false);
	}
}

void UFPGameplayAbility_WeaponFire::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Checked();

	// 델레게이트 등록 해제
	ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(TargetDataSetDelegateHandle);

	// 클라이언트에서 받은 TargetData 소모
	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UFPGameplayAbility_WeaponFire::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
	{
		return false;
	}

	if (AmmoCostTag.IsValid())
	{
		// Check Ammo
		AActor* AvatarActor = ActorInfo->AvatarActor.Get();
		if (AWeapon_Base* Weapon = GetEquippedWeapon(AvatarActor))
		{
			FGameplayTagStackContainer& WeaponTagStacks = Weapon->GetTagStacks();
			return WeaponTagStacks.GetStackCount(AmmoCostTag) > 0;
		}	
	}

	return true;
}

void UFPGameplayAbility_WeaponFire::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	if (HasAuthority(&CurrentActivationInfo))
	{
		if (AmmoCostTag.IsValid())
		{
			if (AWeapon_Base* Weapon = GetEquippedWeapon())
			{
				// Consume Ammo
				FGameplayTagStackContainer& WeaponTagStacks = Weapon->GetTagStacks();
				const int32 NewAmmo = WeaponTagStacks.GetStackCount(AmmoCostTag) - 1;
				WeaponTagStacks.AddTagStack(AmmoCostTag, NewAmmo);
			}	
		}
	}
}

AWeapon_Base* UFPGameplayAbility_WeaponFire::GetEquippedWeapon() const
{
	return GetEquippedWeapon(GetAvatarActorFromActorInfo());
}

AWeapon_Base* UFPGameplayAbility_WeaponFire::GetEquippedWeapon(const AActor* AvatarActor) const
{
	if (UWeaponManageComponent* WeaponManageComp = AvatarActor->FindComponentByClass<UWeaponManageComponent>())
	{
		return WeaponManageComp->GetEquippedWeapon();
	}
	return nullptr;
}

void UFPGameplayAbility_WeaponFire::StartTargeting()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Checked();
	
	FScopedPredictionWindow ScopedPrediction(ASC, CurrentActivationInfo.GetActivationPredictionKey());

	// Trace 결과
	TArray<FHitResult> HitResults;
	PerformLocalTrace(HitResults);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	
	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetDataHandle.Add(NewTargetData);
	}

	OnTargetDataReady(TargetDataHandle, FGameplayTag());
}

void UFPGameplayAbility_WeaponFire::PerformLocalTrace(TArray<FHitResult>& OutHitResults)
{
	// 카메라 기준
	const FTransform TargetingTransform = GetTargetingTransform();
	
	// TargetingTransform 좌표계의 전방 x축 방향
	const FVector AimDir = TargetingTransform.GetUnitAxis(EAxis::X);
	const FVector TraceStart = TargetingTransform.GetTranslation();
	
	// Scatter가 적용된 Trace End
	TArray<FVector> TraceEnds;
	GenerateTraceEndsWithScatterInCartridge(TraceStart, AimDir, TraceEnds);
	
	// Trace 수행
	WeaponTrace(TraceStart, TraceEnds, OutHitResults);
}

FTransform UFPGameplayAbility_WeaponFire::GetTargetingTransform() const
{
	if (const APlayerController* PC = CurrentActorInfo->PlayerController.Get())
	{
		// 카메라
		FVector CamLoc;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamLoc, CamRot);
		
		// 카메라로부터 화면 정중앙을 향하는 방향
		FVector AimDir = CamRot.Vector().GetSafeNormal();
		FVector FocalLoc = CamLoc + AimDir * 1000.f;
		FVector WeaponSourceLoc = GetWeaponTargetingSourceLocation();
		
		// WeaponSourceLoc에서 AimDir 벡터에 수직으로 투영
		// CamLoc을 AimDir 위에 있으면서 WeaponSourceLoc에 가장 가까운 지점으로 업데이트
		CamLoc = FocalLoc + ((WeaponSourceLoc - FocalLoc) | AimDir) * AimDir;

		return FTransform(CamRot, CamLoc);
	}

	// 카메라가 없으면 AvatarActor 기준 사용
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	check(AvatarActor);

	return FTransform(AvatarActor->GetActorForwardVector().Rotation(), AvatarActor->GetActorLocation());
}

FVector UFPGameplayAbility_WeaponFire::GetWeaponTargetingSourceLocation() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	check(AvatarActor);

	AWeapon_Base* Weapon = GetEquippedWeapon(AvatarActor);
	check(Weapon);
	
	return Weapon->GetWeaponTargetingSourceLocation();
}

void UFPGameplayAbility_WeaponFire::GenerateTraceEndsWithScatterInCartridge(const FVector& TraceStart, const FVector& AimDir, TArray<FVector>& OutTraceEnds) const
{
	AWeapon_Base* Weapon = GetEquippedWeapon();
	check(Weapon);

	const float MaxDamageRange = Weapon->GetMaxDamageRange();
	const int32 BulletsPerCartridge = Weapon->GetBulletsPerCartridge();

	const float HalfScatterAngle = Weapon->GetHalfScatterAngle();
	const float HalfScatterAngleRad = FMath::DegreesToRadians(HalfScatterAngle);

	for (int32 Index = 0; Index < BulletsPerCartridge; ++Index)
	{
		FVector FinalAimDir = FMath::VRandCone(AimDir, HalfScatterAngleRad);
		FVector TraceEnd = TraceStart + FinalAimDir * MaxDamageRange;
		OutTraceEnds.Add(TraceEnd);
	}
}

void UFPGameplayAbility_WeaponFire::WeaponTrace(const FVector& TraceStart, const TArray<FVector>& TraceEnds, TArray<FHitResult>& OutHitResults) const
{
	if (!GetWorld())
	{
		return;
	}
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	check(AvatarActor);
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	for (const FVector& TraceEnd : TraceEnds)
	{
		FHitResult HitResult;
		GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity, ECC_Projectile, FCollisionShape::MakeSphere(BulletTraceRadius), QueryParams);
		OutHitResults.Add(HitResult);
	}
}

void UFPGameplayAbility_WeaponFire::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	AWeapon_Base* Weapon = GetEquippedWeapon();
	check(Weapon);
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Checked();

	FScopedPredictionWindow ScopedPrediction(ASC);

	// 이 함수에서만 사용할 수 있도록 소유권 이전
	FGameplayAbilityTargetDataHandle LocalTargetData = MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData));

	// 서버에게 클라이언트 로컬 플레이어의 TargetData 전송
	if (IsLocallyControlled() && !HasAuthority(&CurrentActivationInfo))
	{
		ASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetData, ApplicationTag, ASC->GetPredictionKeyForNewAction());
	}

	// Check ammo
	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		// Server Only
		ApplyDamageToTarget(InData);

		// Target에 적중한 위치의 배열
		TArray<FVector_NetQuantize> ImpactPoints;
		// Target에 적중하지 않은 끝 위치의 배열
		TArray<FVector_NetQuantize> EndPoints;

		for (int32 Index = 0; Index < LocalTargetData.Num(); ++Index)
		{
			if (FGameplayAbilityTargetData_SingleTargetHit* TargetHit = static_cast<FGameplayAbilityTargetData_SingleTargetHit*>(LocalTargetData.Get(Index)))
			{
				if (TargetHit->HitResult.bBlockingHit)
				{
					ImpactPoints.Add(TargetHit->GetEndPoint());
				}
				else
				{
					EndPoints.Add(TargetHit->HitResult.TraceEnd);
				}
			}
		}

		if (IsLocallyControlled())
		{
			// 클라이언트 로컬에서 Weapon Fire Effects를 바로 표시
			Weapon->TriggerWeaponFireEffects(ImpactPoints, EndPoints);
		}
		else
		{
			// 클라이언트의 다른 플레이어의 Weapon Fire Effects를 Replicate해 표시
			Weapon->BroadcastWeaponFireEffects(ImpactPoints, EndPoints);
		}
	}
	else
	{
		// TODO : Reload, Dry fire
		NET_LOG(GetAvatarActorFromActorInfo(), LogTemp, Warning, TEXT("Can't Commit Ability"));
	}

	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UFPGameplayAbility_WeaponFire::ApplyDamageToTarget(const FGameplayAbilityTargetDataHandle& InData) const
{
	if (InData.Num() <= 0 || !HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	// TODO
	// Damage Effect 구현 및 필요한 데이터 설정
	// 데미지를 입히기에 적절한 Target인지 체크

	if (!DamageEffectClass)
	{
		UE_LOG(LogFP, Error, TEXT("[%hs] Can't apply damage because DamageEffectClass is nullptr."), __FUNCTION__);
		return;
	}

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, InData);
}
