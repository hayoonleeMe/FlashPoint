// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FPGameplayAbility.generated.h"

/**
 * 어빌리티를 활성화하는 방법을 정의하는 enum
 */
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	Manual,
	
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnGiveOrSpawn
};

/**
 * Base Gameplay Ability class 
 */
UCLASS(Abstract)
class FLASHPOINT_API UFPGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFPGameplayAbility();

	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	UPROPERTY(EditDefaultsOnly)
	EAbilityActivationPolicy ActivationPolicy;
};
