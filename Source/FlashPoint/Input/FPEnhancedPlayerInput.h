// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedPlayerInput.h"
#include "FPEnhancedPlayerInput.generated.h"

struct FGameplayTag;

/**
 * 
 */
UCLASS()
class FLASHPOINT_API UFPEnhancedPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()

public:
	// InputTag에 해당하는 InputAction의 Key 입력을 Flush한다.
	void FlushPressedInput(const FGameplayTag& InputTag);
};
