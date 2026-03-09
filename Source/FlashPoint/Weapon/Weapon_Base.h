// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Attachment/AttachmentOwnerInterface.h"
#include "GameFramework/Actor.h"
#include "Weapon_Base.generated.h"

class UCustomFovComponent;
class UWeaponAttachmentComponent;
enum class EAttachmentSlot : uint8;
class UFPWeaponConfigData;
class UAbilitySystemComponent;
class UNiagaraComponent;

/**
 * 무기를 정의하는 기본 액터 클래스
 */
UCLASS()
class FLASHPOINT_API AWeapon_Base : public AActor, public IAttachmentOwnerInterface
{
	GENERATED_BODY()

public:
	AWeapon_Base();
	
	// Begin IAttachmentOwnerInterface
	virtual UMeshComponent* GetAttachmentOwnerMeshComponent() const override;
	// End IAttachmentOwnerInterface
	
	virtual void Destroyed() override;

	FGameplayTag GetWeaponTypeTag() const { return WeaponTypeTag; }
	UFPWeaponConfigData* GetWeaponConfigData() const { return WeaponConfigData; }
	
	void SetServerRemainAmmo(int32 InServerRemainAmmo) { ServerRemainAmmo = InServerRemainAmmo; }
	int32 GetServerRemainAmmo() const { return ServerRemainAmmo; }
	
	// Distance에 따른 감소를 적용한 데미지를 반환한다.
	float GetDamageByDistance(float Distance) const;

	// Targeting 할 때 Weapon에서의 Source Location을 반환한다. (보통 총구)
	FVector GetWeaponTargetingSourceLocation() const;

	// LeftHandAttachSocketName에 해당하는 소켓의 Transform을 반환한다.
	FTransform GetLeftHandAttachTransform() const;

	// 무기가 장착된 후 호출된다.
	virtual void OnEquipped();

	// 무기가 장착 해제된 후 호출된다.
	virtual void OnUnEquipped();

	// 모든 클라이언트의 Simulated Proxy 캐릭터가 장착 중인 무기의 Weapon Fire Effects를 트리거
	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastWeaponFireEffects(const TArray<FVector_NetQuantize>& ImpactPoints, const TArray<FVector_NetQuantize>& EndPoints);

	// 로컬에서 캐릭터가 장착 중인 무기의 Weapon Fire Effects를 트리거
	void TriggerWeaponFireEffects(const TArray<FVector_NetQuantize>& ImpactPoints, const TArray<FVector_NetQuantize>& EndPoints);

	void GetFirstPersonRightHandOffset(FVector& OutLoc, FRotator& OutRot) const;
	
	FTransform GetAimDownSightSocketTransform() const;
	float GetAimDownSightFOV() const;
	float GetAimDownSightWeaponFOV() const;
	float GetAimDownSightSpeedModifier() const;
	float GetTimeToADS() const;
	
	void StartAimDownSight();
	void StopAimDownSight();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UAbilitySystemComponent* GetOwnerASC() const;

	template<class T>
	T* GetOwnerASC() const
	{
		return Cast<T>(GetOwnerASC());
	}
	
	bool IsOwnerLocallyControlled() const;

	// 일정 시간 뒤 무기를 다시 표시할 때 사용
	FTimerHandle ShowWeaponTimerHandle;
	
	// 무기 메시를 표시하거나 숨긴다.
	virtual void ShowWeapon(bool bShow);

	// ============================================================================
	// Fire Effects
	// ============================================================================

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TracerComponent;

	// Tracer를 표시하는 Trigger 변수
	// 처음 true로 설정된 뒤, 값이 바뀔 때마다 Tracer를 표시 
	bool bTracerTrigger = false;

	// ============================================================================
	// Weapon Config
	// ============================================================================

	// 무기 타입을 나타내는 태그
	UPROPERTY(EditDefaultsOnly, Category="FlashPoint|Weapon Config")
	FGameplayTag WeaponTypeTag;

	UPROPERTY(Transient)
	TObjectPtr<UFPWeaponConfigData> WeaponConfigData;

	// 현재 탄창에 남은 총알 수
	// 서버에서만 유효하다.
	int32 ServerRemainAmmo = 0;
	
	// ============================================================================
	// Attachment
	// ============================================================================
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attachment");
	TObjectPtr<UWeaponAttachmentComponent> WeaponAttachmentComponent;
	
	void OnAttachmentAdded(EAttachmentSlot AttachmentSlot, AActor* AttachmentActor);
	void OnAttachmentRemoved(EAttachmentSlot AttachmentSlot, AActor* AttachmentActor);

	// ============================================================================
	// CustomFOV
	// ============================================================================
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="CustomFOV")
	TObjectPtr<UCustomFovComponent> CustomFovComponent;
};
