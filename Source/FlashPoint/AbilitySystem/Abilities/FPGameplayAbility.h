// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FPGameplayAbility.generated.h"

/**
 * Base Gameplay Ability class 
 */
UCLASS(Abstract)
class FLASHPOINT_API UFPGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFPGameplayAbility();

	// 해당 어빌리티의 Input Tag를 반환한다.
	FGameplayTag GetInputTag() const;

	// 해당 어빌리티의 InputTag에 연동된 입력을 Flush한다.
	void FlushPressedInput() const;

	// InputTag에 연동된 입력을 Flush한다.
	void FlushPressedInput(const FGameplayTag& InputTag) const;

	APlayerController* GetPlayerController() const;
};
