// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "System/GameplayTagStackContainer.h"
#include "WeaponManageComponent.generated.h"

class AWeapon_Base;

// 장착 중인 무기가 변경될 때를 알리는 델레게이트
// 서버와 클라 모두에서 호출된다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, AWeapon_Base*);

// 무기 장착 상태 변경을 달리는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponEquipStateChangedDelegate, int32/*ActiveSlotIndex*/, AWeapon_Base*/*EquippedWeapon*/);

// AmmoTagStacks가 변경될 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmmoTagStackChangedDelegate, const FGameplayTag&/*Tag*/, int32/*StackCount*/);

/**
 * 무기를 관리하는 액터 컴포넌트
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UWeaponManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponManageComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;

	AWeapon_Base* GetEquippedWeapon() const { return EquippedWeapon; }

	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	FOnWeaponEquipStateChangedDelegate OnWeaponEquipStateChangedDelegate;
	
	FGameplayTagStackContainer& GetAmmoTagStacks() { return AmmoTagStacks; }

	// Actor가 장착 중인 무기 타입에 맞는 ReserveAmmo Stack Count를 반환한다.
	// Actor가 유효하지 않거나 무기를 장착하고 있지 않으면 0을 반환한다.
	static int32 GetReserveAmmoStackCount(const AActor* Actor);

	// AmmoTagStacks에서 Tag의 Stack Count가 변경될 때 InDelegate를 브로드캐스트하도록 등록한다.
	void RegisterAmmoTagStackChangedEvent(const FGameplayTag& Tag, const FOnAmmoTagStackChangedDelegate::FDelegate& InDelegate);

	// 모든 무기 타입의 ReserveAmmo의 Stack Count가 변경될 때 InDelegate를 브로드캐스트하도록 등록한다.
	void RegisterAllReserveAmmoChangedEvent(const FOnAmmoTagStackChangedDelegate::FDelegate& InDelegate);

	bool HasAuthority() const;

	// SlotNumber 슬롯의 무기를 장착한다.
	UFUNCTION(Server, Reliable, BlueprintCallable) 	// TODO : BlueprintCallable for Test
	void ServerEquipWeaponAtSlot(int32 SlotNumber);

	// 현재 슬롯에 새 무기를 장착한다.
	// Server Only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)	// TODO : UFUNCTION() for Test
	void EquipNewWeapon(const TSubclassOf<AWeapon_Base>& WeaponToEquipClass);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerUnEquipWeapon();

protected:
	virtual void BeginPlay() override;
	
private:
	// WeaponClass 무기 액터를 생성하고 장착한다.
	// Server Only
	void EquipWeaponInternal(const TSubclassOf<AWeapon_Base>& WeaponClass);

	// 기존 슬롯에 존재하는 WeaponInSlot을 장착한다.
	// Server Only
	void EquipWeaponInternal(AWeapon_Base* WeaponInSlot);

	// 현재 장착된 무기를 장착 해제
	// Server Only
	void UnEquipWeapon(bool bDestroy);

	// 각 슬롯에 저장된 무기 액터를 저장
	// 서버에서만 존재
	UPROPERTY()
	TArray<TObjectPtr<AWeapon_Base>> WeaponSlots;

	// 무기 슬롯 수
	UPROPERTY(EditDefaultsOnly)
	int32 NumSlots;

	// 현재 활성화중인 슬롯 인덱스
	UPROPERTY(Replicated)
	int32 ActiveSlotIndex;

	// ActiveSlotIndex에서 장착 중인 무기
	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	TObjectPtr<AWeapon_Base> EquippedWeapon;

	UFUNCTION()
	void OnRep_EquippedWeapon(AWeapon_Base* UnEquippedWeapon);

	// 슬롯 변경, 장착 무기 변경 등 Equip State 변경을 알리기 위한 트리거 프로퍼티 
	UPROPERTY(ReplicatedUsing=OnRep_WeaponEquipStateUpdateCounter)
	uint8 WeaponEquipStateUpdateCounter;

	UFUNCTION()
	void OnRep_WeaponEquipStateUpdateCounter();

	// ============================================================================
	// Tag Stacks
	// ============================================================================

	// 초기 소지 Reserve Ammo
	// key: Weapon Type Tag, value: Count 
	UPROPERTY(EditDefaultsOnly, meta=(Categories="Weapon.Type"))
	TMap<FGameplayTag, int32> InitialReserveAmmoMap;
	
	// 탄약 관련 TagStack을 관리하는 Container
	// Ammo 태그의 Count로 현재 장착한 무기의 탄약 수를 나타낸다.
	// Weapon Type 태그의 Count로 해당 무기의 ReserveAmmo를 나타낸다.
	UPROPERTY(Replicated)
	FGameplayTagStackContainer AmmoTagStacks;

	// AmmoTagStacks 변경이 발생할 때, 이 맵에 key로 등록된 태그라면 value로 등록된 델레게이트를 브로드캐스트한다.
	TMap<FGameplayTag, FOnAmmoTagStackChangedDelegate> AmmoTagStackChangedEventMap;

	// AmmoTagStack 변경이 발생하면 호출되는 Callback
	void OnAmmoTagStackChanged(const FGameplayTag& Tag, int32 StackCount);
};
