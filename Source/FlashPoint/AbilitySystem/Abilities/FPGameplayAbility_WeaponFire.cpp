// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_WeaponFire.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FlashPoint.h"
#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/FPGameplayAbilityTargetData_SingleTargetHit.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_WeaponFire)

UFPGameplayAbility_WeaponFire::UFPGameplayAbility_WeaponFire()
{
	AbilityTags.AddTag(FPGameplayTags::Ability_WeaponFire);
	ActivationOwnedTags.AddTag(FPGameplayTags::CharacterState_IsFiring);
	ActivationBlockedTags.AddTag(FPGameplayTags::Weapon_NoFire);
	AmmoCostTag = FPGameplayTags::Weapon_Data_Ammo;

	ScatterDistribution = 1.f;
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
	bEndWhenInputReleased = false;
	bInputReleased = false;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo_Checked();

	// 델레게이트 등록
	TargetDataSetDelegateHandle = ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReady);

	// Input Release Event 등록
	if (UAbilityTask_WaitInputRelease* Task = UAbilityTask_WaitInputRelease::WaitInputRelease(this))
	{
		Task->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		Task->ReadyForActivation();
	}

	UWorld* World = GetWorld();
	check(World);
	
	if (bAutoFire)
	{
		World->GetTimerManager().SetTimer(FireDelayTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::AutoFire), FireDelay, true, 0.f);
	}
	else
	{
		SemiAutoFire();
	}
}

void UFPGameplayAbility_WeaponFire::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (GetWorld())
	{
		// Fire Delay Timer 해제
		GetWorld()->GetTimerManager().ClearTimer(FireDelayTimerHandle);
	}
	
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

void UFPGameplayAbility_WeaponFire::Fire()
{
	// Check ammo
	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		if (IsLocallyControlled())
		{
			StartTargeting();
		}

		if (CharacterFireMontage)
		{
			// Play Character Fire Anim Montage
			if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("Play Character Fire Montage"), CharacterFireMontage))
			{
				MontageTask->ReadyForActivation();
			}
		}
	}
	else
	{
		// TODO : Reload, Dry fire
		NET_LOG(GetAvatarActorFromActorInfo(), LogTemp, Warning, TEXT("Can't Commit Ability"));
	}
}

void UFPGameplayAbility_WeaponFire::AutoFire()
{
	if (bInputReleased)
	{
		K2_EndAbility();
		return;
	}
	
	Fire();
}

void UFPGameplayAbility_WeaponFire::SemiAutoFire()
{
	Fire();

	// FireDelay가 지나면 종료
	GetWorld()->GetTimerManager().SetTimer(FireDelayTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnSemiAutoFireDelayEnded), FireDelay, false);
}

void UFPGameplayAbility_WeaponFire::OnSemiAutoFireDelayEnded()
{
	if (bInputReleased)	
	{
		K2_EndAbility();
	}
	else
	{
		// 입력이 떼어지면 종료시킴
		bEndWhenInputReleased = true;
	}
}

void UFPGameplayAbility_WeaponFire::OnInputReleased(float TimeHeld)
{
	bInputReleased = true;
	if (bEndWhenInputReleased)
	{
		K2_EndAbility();
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

bool UFPGameplayAbility_WeaponFire::CanApplyDamage(const AActor* TargetActor)
{
	if (IsValid(TargetActor))
	{
		// TODO : 적인지 체크
		return TargetActor->IsA<ACharacter>();
	}
	return false;
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
		FFPGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FFPGameplayAbilityTargetData_SingleTargetHit(HitResult);
		NewTargetData->bCanApplyDamage = CanApplyDamage(HitResult.GetActor()); 
		TargetDataHandle.Add(NewTargetData);
	}

	OnTargetDataReady(TargetDataHandle, FGameplayTag());
}

void UFPGameplayAbility_WeaponFire::PerformLocalTrace(TArray<FHitResult>& OutHitResults)
{
	const FVector TraceStart = GetWeaponTargetingSourceLocation();
	const FVector TargetLoc = GetTargetLocation();
	
	// Scatter가 적용된 Trace End
	TArray<FVector> TraceEnds;
	GenerateTraceEndsWithScatterInCartridge(TraceStart, TargetLoc, TraceEnds);
	
	// Trace 수행
	WeaponTrace(TraceStart, TraceEnds, OutHitResults);
}

FVector UFPGameplayAbility_WeaponFire::GetTargetLocation() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	check(AvatarActor);
	
	APlayerController* OwnerController = GetActorInfo().PlayerController.Get();
	check(OwnerController);

	AWeapon_Base* Weapon = GetEquippedWeapon();
	check(Weapon);

	const float MaxDamageRange = Weapon->GetMaxDamageRange();
	
	// Viewport Size
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	// Viewport 정중앙의 크로스헤어 위치 계산 (Viewport space = screen space)
	const FVector2D CrosshairLocation(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f);

	// Crosshair를 World Space로 변환
	FVector CrosshairWorldPos;
	FVector CrosshairWorldDir;
	UGameplayStatics::DeprojectScreenToWorld(OwnerController, CrosshairLocation, CrosshairWorldPos, CrosshairWorldDir);

	// 총구에서 가장 가까운 CrosshairWorldDirection 벡터 상의 위치에서 Trace 수행
	const FVector TraceStart = CrosshairWorldPos + ((GetWeaponTargetingSourceLocation() - CrosshairWorldPos) | CrosshairWorldDir) * CrosshairWorldDir;
	const FVector TraceEnd = TraceStart + CrosshairWorldDir * MaxDamageRange;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Projectile, QueryParams);

	return HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd;
}

FVector UFPGameplayAbility_WeaponFire::GetWeaponTargetingSourceLocation() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	check(AvatarActor);

	AWeapon_Base* Weapon = GetEquippedWeapon(AvatarActor);
	check(Weapon);
	
	return Weapon->GetWeaponTargetingSourceLocation();
}

void UFPGameplayAbility_WeaponFire::GenerateTraceEndsWithScatterInCartridge(const FVector& TraceStart, const FVector& TargetLoc, TArray<FVector>& OutTraceEnds) const
{
	AWeapon_Base* Weapon = GetEquippedWeapon();
	check(Weapon);

	const float MaxDamageRange = Weapon->GetMaxDamageRange();
	const int32 BulletsPerCartridge = Weapon->GetBulletsPerCartridge();

	for (int32 Index = 0; Index < BulletsPerCartridge; ++Index)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::Pow(FMath::FRand(), ScatterDistribution) * MaxScatterAmount;
		const FVector TargetLocWithScatter = TargetLoc + RandVec;
		const FVector WeaponAimDir = (TargetLocWithScatter - TraceStart).GetSafeNormal();
		const FVector TraceEnd = TraceStart + WeaponAimDir * MaxDamageRange;
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

	// Server Only
	ApplyDamageToTarget(LocalTargetData);

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

	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UFPGameplayAbility_WeaponFire::ApplyDamageToTarget(FGameplayAbilityTargetDataHandle& InData) const
{
	if (InData.Num() <= 0 || !HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	if (!DamageEffectClass)
	{
		UE_LOG(LogFP, Error, TEXT("[%hs] Can't apply damage because DamageEffectClass is nullptr."), __FUNCTION__);
		return;
	}

	const AWeapon_Base* Weapon = GetEquippedWeapon();
	check(Weapon);

	for (int32 Index = 0; Index < InData.Num(); ++Index)
	{
		if (FFPGameplayAbilityTargetData_SingleTargetHit* HitData = static_cast<FFPGameplayAbilityTargetData_SingleTargetHit*>(InData.Get(Index)))
		{
			// 데미지를 입힐 대상인지가 bHitReplaced에 저장됨
			if (HitData->bCanApplyDamage)
			{
				float BaseDamage = Weapon->GetDamageByDistance(HitData->HitResult.Distance);

				const bool bHeadShot = HitData->HitResult.BoneName == TEXT("head");
				if (bHeadShot)
				{
					BaseDamage *= Weapon->GetHeadShotMultiplier();
					NET_LOG(GetAvatarActorFromActorInfo(), LogTemp, Warning, TEXT("HeadShot"));
				}

				// 소수점 둘째 자리까지 반올림
				BaseDamage = FMath::RoundToFloat(BaseDamage * 100.f) / 100.f;

				// TODO : Indicates Damage and Update HUD

				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
				EffectSpecHandle.Data->SetSetByCallerMagnitude(FPGameplayTags::Attributes_IncomingDamage, BaseDamage);
				
				HitData->ApplyGameplayEffectSpec(*EffectSpecHandle.Data);
			}
		}
	}
}
