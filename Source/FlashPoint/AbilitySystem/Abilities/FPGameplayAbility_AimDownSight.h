// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGameplayAbility.h"
#include "FPGameplayAbility_AimDownSight.generated.h"

class UFPWeaponConfigData;
class AWeapon_Base;

/**
 * Aim Down Sight를 수행하는 어빌리티
 */
UCLASS()
class FLASHPOINT_API UFPGameplayAbility_AimDownSight : public UFPGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFPGameplayAbility_AimDownSight();
	
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<AWeapon_Base> EquippedWeaponWeakPtr;
	
	UPROPERTY()
	TObjectPtr<UFPWeaponConfigData> EquippedWeaponConfigData;
};
