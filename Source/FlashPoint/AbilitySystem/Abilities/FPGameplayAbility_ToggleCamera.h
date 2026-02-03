// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGameplayAbility.h"
#include "FPGameplayAbility_ToggleCamera.generated.h"

/**
 * 로컬에서 카메라 시점을 변경하는 어빌리티 (1인칭 - 3인칭)
 */
UCLASS()
class FLASHPOINT_API UFPGameplayAbility_ToggleCamera : public UFPGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFPGameplayAbility_ToggleCamera();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
