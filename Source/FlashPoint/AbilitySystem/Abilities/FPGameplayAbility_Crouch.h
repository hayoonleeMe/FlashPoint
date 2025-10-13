// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPGameplayAbility.h"
#include "FPGameplayAbility_Crouch.generated.h"

/**
 * 캐릭터 Crouch를 수행하는 어빌리티
 */
UCLASS()
class FLASHPOINT_API UFPGameplayAbility_Crouch : public UFPGameplayAbility
{
	GENERATED_BODY()

public:
	UFPGameplayAbility_Crouch();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
