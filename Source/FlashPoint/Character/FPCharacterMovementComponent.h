// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPCharacterMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UFPCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Crouch 체크 무시하도록 오버라이드
	virtual bool CanAttemptJump() const override;
};
