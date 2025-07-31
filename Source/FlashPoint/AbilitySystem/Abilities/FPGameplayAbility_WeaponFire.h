// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGameplayAbility.h"
#include "FPGameplayAbility_WeaponFire.generated.h"

class AWeapon_Base;

/**
 * 현재 장착 중인 총기 무기를 발사하는 Gameplay Ability
 * 발사를 시작하면 어빌리티가 실행되고, 실제 발사가 끝나면 어빌리티가 종료된다.
 */
UCLASS()
class FLASHPOINT_API UFPGameplayAbility_WeaponFire : public UFPGameplayAbility
{
	GENERATED_BODY()

public:
	UFPGameplayAbility_WeaponFire();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	// 실제 발사를 수행한다.
	virtual void Fire();

	void AutoFire();
	void SemiAutoFire();
	void OnSemiAutoFireDelayEnded();

	// true라면, 입력 키가 떼어졌을 때 어빌리티를 종료한다.
	bool bEndWhenInputReleased = false;
	// 입력 키가 떼어졌는지 여부를 나타낸다.
	bool bInputReleased = false;

	// Wait Input Released에 등록된 콜백 함수
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	// 현재 플레이어 캐릭터가 장착 중인 Weapon을 반환한다.
	AWeapon_Base* GetEquippedWeapon() const;
	AWeapon_Base* GetEquippedWeapon(const AActor* AvatarActor) const;

	// true라면 연사를 수행하고, false라면 단발 사격을 수행한다.
	UPROPERTY(EditDefaultsOnly)
	uint8 bAutoFire : 1;

	// 발사 간 딜레이
	UPROPERTY(EditDefaultsOnly)
	float FireDelay;

	FTimerHandle FireDelayTimerHandle;

	// Sweep Trace를 수행할 때 사용할 Sphere Radius 
	UPROPERTY(EditDefaultsOnly)
	float BulletTraceRadius;

	// 총알이 최대로 퍼질 수 있는 정도
	UPROPERTY(EditDefaultsOnly)
	float MaxScatterAmount;

	// 총알이 퍼지는 정도를 제어하는 값
	// 값이 클수록 총알이 중앙으로 집중된다.
	UPROPERTY(EditDefaultsOnly)
	float ScatterDistribution;

	// Character의 Weapon Fire Anim Montage
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> CharacterFireMontage;

	// Damage GameplayEffect Class
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 플레이어 캐릭터가 TargetActor를 공격해 데미지를 입힐 수 있는지 체크
	static bool CanApplyDamage(const AActor* TargetActor);

	// Cost로 소모할 Ammo를 나타내는 GameplayTag
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AmmoCostTag;

private:
	// 플레이어 로컬에서 Target을 구해 TargetData를 설정한다.
	void StartTargeting();

	// 플레이어 로컬에서 Trace를 수행해 Target을 구한다.
	void PerformLocalTrace(TArray<FHitResult>& OutHitResults);

	// 총구에서 총알이 향할 위치를 계산해 반환한다.
	FVector GetTargetLocation() const;
	
	// Weapon의 Targeting Source Location을 반환한다. (보통 총구)
	FVector GetWeaponTargetingSourceLocation() const;

	// 하나의 탄약에 포함된 총알 수만큼, Scatter를 적용한 Trace End 배열을 생성한다.
	void GenerateTraceEndsWithScatterInCartridge(const FVector& TraceStart, const FVector& TargetLoc, TArray<FVector>& OutTraceEnds) const;

	// 모든 TraceEnds를 향하는 LineTrace를 수행한 결과를 OutHitResults에 저장한다.
	void WeaponTrace(const FVector& TraceStart, const TArray<FVector>& TraceEnds, TArray<FHitResult>& OutHitResults) const;

	FDelegateHandle TargetDataSetDelegateHandle;

	// TargetData가 준비되면 호출되는 콜백 함수
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	// TargetActors들에게 Damage Effect를 적용한다.
	void ApplyDamageToTarget(FGameplayAbilityTargetDataHandle& InData) const;
};
