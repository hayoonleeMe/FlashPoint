// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility_WeaponFire.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FlashPoint.h"
#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/BasePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_WeaponFire)

UFPGameplayAbility_WeaponFire::UFPGameplayAbility_WeaponFire()
{
	AbilityTags.AddTag(FPGameplayTags::Ability::WeaponFire);
	ActivationOwnedTags.AddTag(FPGameplayTags::CharacterState::IsFiring);
	ActivationBlockedTags.AddTag(FPGameplayTags::Weapon::NoFire);
	
	// Sprint 중 발사 시 Sprint 취소
	CancelAbilitiesWithTag.AddTag(FPGameplayTags::Ability::Sprint);

	AmmoCostTag = FPGameplayTags::Weapon::Data::Ammo;

	SpreadCalcDistance = 1500.f;
	SpreadDistribution = 1.f;
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
		World->GetTimerManager().SetTimer(FireDelayTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::AutoFire), FireDelay, true, 0);
	}
	else
	{
		Fire();
		World->GetTimerManager().SetTimer(FireDelayTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::K2_EndAbility), FireDelay, false);	
	}
}

void UFPGameplayAbility_WeaponFire::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!bAutoFire)
	{
		// SemiAuto의 경우, 발사가 끝나고 입력을 Release 후 Trigger 하도록 Flush 
		FlushPressedInput();
	}
	
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
		if (AActor* AvatarActor = ActorInfo->AvatarActor.Get())
		{
			if (UWeaponManageComponent* WeaponManageComponent = AvatarActor->FindComponentByClass<UWeaponManageComponent>())
			{
				FGameplayTagStackContainer& AmmoTagStacks = WeaponManageComponent->GetAmmoTagStacks();
				const bool bHasAmmo = AmmoTagStacks.GetStackCount(AmmoCostTag) > 0;
				if (!bHasAmmo && OptionalRelevantTags)
				{
					OptionalRelevantTags->AddTag(FPGameplayTags::Ability::Fail::NoAmmo);
				}
				return bHasAmmo;
			}
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
			if (AActor* AvatarActor = ActorInfo->AvatarActor.Get())
			{
				if (UWeaponManageComponent* WeaponManageComponent = AvatarActor->FindComponentByClass<UWeaponManageComponent>())
				{
					// Consume Ammo
					FGameplayTagStackContainer& AmmoTagStacks = WeaponManageComponent->GetAmmoTagStacks();
					const int32 NewAmmo = AmmoTagStacks.GetStackCount(AmmoCostTag) - 1;
					AmmoTagStacks.AddTagStack(AmmoCostTag, NewAmmo);
				}
			}
		}
	}
}

void UFPGameplayAbility_WeaponFire::Fire()
{
	// 발사 중 NoFire 태그가 추가되면 발사 종료
	if (GetAbilitySystemComponentFromActorInfo_Checked()->HasMatchingGameplayTag(FPGameplayTags::Weapon::NoFire))
	{
		K2_EndAbility();
		return;
	}
	
	// Check ammo
	if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		if (IsLocallyControlled())
		{
			StartTargeting();
			ApplyRecoil();
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
		if (bAutoFire)
		{
			// 연사를 사용하는 총의 경우, CheckCost()를 통과하지 못하면 입력을 끊고 종료한다.
			FlushPressedInput();
			K2_EndAbility();
		}
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

void UFPGameplayAbility_WeaponFire::OnInputReleased(float TimeHeld)
{
	bInputReleased = true;
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

bool UFPGameplayAbility_WeaponFire::CanApplyDamage(const AActor* TargetActor) const
{
	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		if (const ABasePlayerState* TargetPS = TargetPawn->GetPlayerState<ABasePlayerState>())
		{
			const ETeam TargetTeam = TargetPS->GetTeam();
			
			if (const APlayerController* PC = GetPlayerController())
			{
				if (const ABasePlayerState* InstigatorPS = PC->GetPlayerState<ABasePlayerState>())
				{
					const ETeam InstigatorTeam = InstigatorPS->GetTeam();

					// 개인전이면 팀이 None일 때, 팀전이면 팀이 None이 아니고 다른 팀일 떄 공격할 수 있다.
					const bool bCanAttackInFFA = InstigatorTeam == ETeam::None && InstigatorTeam == TargetTeam;
					const bool bCanAttackInTDM = InstigatorTeam != ETeam::None && InstigatorTeam != TargetTeam;
					
					return bCanAttackInFFA || bCanAttackInTDM;
				}
			}
		}
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
		TargetDataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));
	}

	OnTargetDataReady(TargetDataHandle, FGameplayTag());
}

void UFPGameplayAbility_WeaponFire::PerformLocalTrace(TArray<FHitResult>& OutHitResults)
{
	const FVector TraceStart = GetWeaponTargetingSourceLocation();
	const FVector TargetLoc = GetTargetLocation();
	
	// Spread가 적용된 Trace End
	TArray<FVector> TraceEnds;
	GenerateTraceEndsWithSpreadInCartridge(TraceStart, TargetLoc, TraceEnds);
	
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

void UFPGameplayAbility_WeaponFire::GenerateTraceEndsWithSpreadInCartridge(const FVector& TraceStart, const FVector& TargetLoc, TArray<FVector>& OutTraceEnds) const
{
	const UWeaponManageComponent* WeaponManageComponent = UWeaponManageComponent::Get(GetAvatarActorFromActorInfo());
	if (!WeaponManageComponent)
	{
		return;
	}
	
	AWeapon_Base* Weapon = GetEquippedWeapon();
	check(Weapon);
	
	const int32 BulletsPerCartridge = Weapon->GetBulletsPerCartridge();
	const float TotalSpreadAmount = WeaponManageComponent->GetCurrentAimSpread();
	const float MaxDamageRange = Weapon->GetMaxDamageRange();
	const FVector TargetDir = (TargetLoc - TraceStart).GetSafeNormal();
	const FVector BaseSpreadCalcLoc = TraceStart + TargetDir * SpreadCalcDistance;

	for (int32 Index = 0; Index < BulletsPerCartridge; ++Index)
	{
		FVector WeaponAimDir = TargetDir;
		if (TotalSpreadAmount > 0.f)
		{
			FVector RandVec = UKismetMathLibrary::RandomUnitVector() * TotalSpreadAmount;
			if (SpreadDistribution > 1.f)
			{
				RandVec *= FMath::Pow(FMath::FRand(), SpreadDistribution);
			}
			const FVector SpreadLoc = BaseSpreadCalcLoc + RandVec;
			WeaponAimDir = (SpreadLoc - TraceStart).GetSafeNormal();
		}
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
		if (FGameplayAbilityTargetData_SingleTargetHit* HitData = static_cast<FGameplayAbilityTargetData_SingleTargetHit*>(InData.Get(Index)))
		{
			if (const AActor* TargetActor = HitData->HitResult.GetActor())
			{
				if (CanApplyDamage(TargetActor))
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
					EffectSpecHandle.Data->SetSetByCallerMagnitude(FPGameplayTags::Attributes::IncomingDamage, BaseDamage);
				
					HitData->ApplyGameplayEffectSpec(*EffectSpecHandle.Data);
				}
			}
		}
	}
}

void UFPGameplayAbility_WeaponFire::ApplyRecoil() const
{
	if (UWeaponManageComponent* WeaponManageComponent = UWeaponManageComponent::Get(GetAvatarActorFromActorInfo()))
	{
		WeaponManageComponent->ApplyRecoil();
	}
}
