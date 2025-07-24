// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponManageComponent.generated.h"

class AWeapon_Base;

// 장착 중인 무기가 변경될 때를 알리는 델레게이트
// 서버와 클라 모두에서 호출된다.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, AWeapon_Base*);

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
};
