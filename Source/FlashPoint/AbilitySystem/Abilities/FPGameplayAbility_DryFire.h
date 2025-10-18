// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGameplayAbility.h"
#include "FPGameplayAbility_DryFire.generated.h"

/**
 * Ammo와 ReserveAmmo 모두 없어 DryFire Montage를 재생하는 어빌리티
 */
UCLASS()
class FLASHPOINT_API UFPGameplayAbility_DryFire : public UFPGameplayAbility
{
	GENERATED_BODY()

public:
	UFPGameplayAbility_DryFire();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> DryFireMontage;
};
