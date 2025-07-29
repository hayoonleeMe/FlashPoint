// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "System/GameplayTagStackContainer.h"
#include "Weapon_Base.generated.h"

class UAbilitySystemComponent;
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * 무기 장착에 필요한 정보를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FWeaponEquipInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocketName;
	
	UPROPERTY(EditDefaultsOnly)
	FTransform AttachTransform;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> UnEquipMontage;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CosmeticTags;
};

// TODO : UI Info

/**
 * 무기를 정의하는 기본 액터 클래스
 */
UCLASS()
class FLASHPOINT_API AWeapon_Base : public AActor
{
	GENERATED_BODY()

public:
	AWeapon_Base();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	const FWeaponEquipInfo& GetEquipInfo() const { return EquipInfo; }
	FGameplayTag GetWeaponTypeTag() const { return WeaponTypeTag; }

	float GetMaxDamageRange() const { return MaxDamageRange; }
	int32 GetBulletsPerCartridge() const { return BulletsPerCartridge; }
	float GetHeadShotMultiplier() const { return HeadShotMultiplier; }
	
	// Distance에 따른 감소를 적용한 데미지를 반환한다.
	float GetDamageByDistance(float Distance) const;

	FGameplayTagStackContainer& GetTagStacks() { return TagStacks; }

	// Targeting 할 때 Weapon에서의 Source Location을 반환한다. (보통 총구)
	FVector GetWeaponTargetingSourceLocation() const;

	// 무기가 장착된 후 호출된다.
	virtual void OnEquipped();

	// 무기가 장착 해제된 후 호출된다.
	virtual void OnUnEquipped();

	// 모든 클라이언트의 Simulated Proxy 캐릭터가 장착 중인 무기의 Weapon Fire Effects를 트리거
	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastWeaponFireEffects(const TArray<FVector_NetQuantize>& ImpactPoints, const TArray<FVector_NetQuantize>& EndPoints);

	// 로컬에서 캐릭터가 장착 중인 무기의 Weapon Fire Effects를 트리거
	void TriggerWeaponFireEffects(const TArray<FVector_NetQuantize>& ImpactPoints, const TArray<FVector_NetQuantize>& EndPoints);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	// 무기 타입을 나타내는 태그
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WeaponTypeTag;

	UAbilitySystemComponent* GetOwnerASC() const;

	// ============================================================================
	// Equip
	// ============================================================================
	
	// 장착에 필요한 정보 
	UPROPERTY(EditDefaultsOnly, Category="Equip")
	FWeaponEquipInfo EquipInfo;

	void LinkWeaponAnimLayer(bool bUseDefault) const;
	void PlayOwningCharacterMontage(UAnimMontage* MontageToPlay);

	// EquipMontage or UnEquipMontage가 종료되면 발사를 막는 태그를 제거하도록 등록
	void BindMontageEndedDelegate(UAnimMontage* Montage);
	FOnMontageEnded OnMontageEndedDelegate;

	// 발사를 막는 태그를 업데이트한다.
	void UpdateFireBlockTag(bool bBlockFire) const;

	// 일정 시간 뒤 무기를 다시 표시할 때 사용
	FTimerHandle ShowWeaponTimerHandle;

	// ============================================================================
	// Fire Effects
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TracerComponent;

	UPROPERTY(EditDefaultsOnly, Category="Fire Effects")
	TObjectPtr<UNiagaraSystem> TracerSystem;

	// Tracer를 표시하는 Trigger 변수
	// 처음 true로 설정된 뒤, 값이 바뀔 때마다 Tracer를 표시 
	bool bTracerTrigger = false;

	// ============================================================================
	// Weapon Config
	// ============================================================================

	// 한 번에 발사되는 총알 수 
	UPROPERTY(EditDefaultsOnly, Category="Weapon Config")
	int32 BulletsPerCartridge;

	// 한 탄창 당 총알 수
	UPROPERTY(EditDefaultsOnly, Category="Weapon Config")
	float MagCapacity;

	// 최대 사거리
	UPROPERTY(EditDefaultsOnly, Category="Weapon Config")
	float MaxDamageRange;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Config")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Config")
	TObjectPtr<UCurveFloat> DamageFallOffCurve;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Config")
	float HeadShotMultiplier;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer TagStacks;

	void InitializeTagStacks();
};
