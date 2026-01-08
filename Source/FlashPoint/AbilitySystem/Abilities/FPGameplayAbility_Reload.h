// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGameplayAbility.h"
#include "FPGameplayAbility_Reload.generated.h"

class AWeapon_Base;

/**
 * 현재 장착 중인 무기의 총알을 재장전하는 어빌리티 
 */
UCLASS()
class FLASHPOINT_API UFPGameplayAbility_Reload : public UFPGameplayAbility
{
	GENERATED_BODY()

public:
	UFPGameplayAbility_Reload();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	FDelegateHandle WeaponWallBlockDataChangedDelegateHandle;

	void OnWeaponWallBlockDataChanged(bool bIsWeaponWallBlocked, bool bUseWeaponUp);

	bool bMontageUpdated = false;
	
	FName SelectSectionName(bool bIsWeaponWallBlocked, bool bUseWeaponUp) const;

	UAnimMontage* SelectReloadMontage(bool bIsWeaponWallBlocked, bool bUseWeaponUp) const;

	void PlayReloadMontage(bool bIsUpdate, bool bIsWeaponWallBlocked, bool bUseWeaponUp);

	// AvatarActor가 장착 중인 Weapon을 반환한다.
	AWeapon_Base* GetEquippedWeapon(const AActor* AvatarActor) const;

	UFUNCTION()
	void OnMontageInterrupted();

	// Reload Montage에서 전송한 GameplayEvent를 받을 때 호출되는 Callback
	UFUNCTION()
	void OnReloadFinished(FGameplayEventData Payload);
	
	// 재장전을 수행한다.
	void Reload() const;

	UFUNCTION()
	void OnMagazineOut(FGameplayEventData Payload);

	UFUNCTION()
	void OnMagazineIn(FGameplayEventData Payload);

private:
	// 무기 메시의 Magazine Bone Name
	UPROPERTY(EditDefaultsOnly)
	FName MagazineBoneName;

	// 무기 Magazine 메시를 부착할 캐릭터의 Bone Name
	UPROPERTY(EditDefaultsOnly)
	FName MagazineAttachBoneName;

	// 무기 Magazine 메시를 캐릭터의 Bone에 부착할 때 적용할 Relative Transform
	UPROPERTY(EditDefaultsOnly)
	FTransform MagazineAttachTransform;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> WeaponBlockDownReloadMontage;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> WeaponBlockUpReloadMontage;

	UPROPERTY(EditDefaultsOnly)
	float PlayRate;
};
