// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Weapon_Base.generated.h"

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
	virtual void Destroyed() override;

	const FWeaponEquipInfo& GetEquipInfo() const { return EquipInfo; }
	FGameplayTag GetWeaponTypeTag() const { return WeaponTypeTag; }

	// 무기가 장착된 후 호출된다.
	virtual void OnEquipped();

	// 무기가 장착 해제된 후 호출된다.
	virtual void OnUnEquipped();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category="Equip")
	FWeaponEquipInfo EquipInfo;

	void LinkWeaponAnimLayer(bool bUseDefault) const;
	void PlayMontage(UAnimMontage* MontageToPlay) const;

	// 일정 시간 뒤 무기를 다시 표시할 때 사용
	FTimerHandle ShowWeaponTimerHandle;

	// 무기 타입을 나타내는 태그
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WeaponTypeTag;
};
