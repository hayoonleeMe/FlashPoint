// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCharacterMovementComponent)

bool UFPCharacterMovementComponent::CanAttemptJump() const
{
	// Crouch 체크 무시
	return IsJumpAllowed() &&
		   (IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}
