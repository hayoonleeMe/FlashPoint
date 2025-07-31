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


};
