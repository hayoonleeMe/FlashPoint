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

	// AvatarActor가 장착 중인 Weapon을 반환한다.
	AWeapon_Base* GetEquippedWeapon(const AActor* AvatarActor) const;

	// Reload Montage에서 전송한 GameplayEvent를 받을 때 호출되는 Callback
	UFUNCTION()
	void OnReloadFinished(FGameplayEventData Payload);
	
	// 재장전을 수행한다.
	void Reload() const;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> CharacterReloadMontage;

	UPROPERTY(EditDefaultsOnly)
	float PlayRate;
};
