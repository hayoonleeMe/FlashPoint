// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCharacterMovementComponent)

UFPCharacterMovementComponent::UFPCharacterMovementComponent()
{
	GroundTraceDistance = 100000.f;
}

bool UFPCharacterMovementComponent::CanAttemptJump() const
{
	// Crouch 체크 무시
	return IsJumpAllowed() &&
		   (IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

float UFPCharacterMovementComponent::GetGroundDistance() const
{
	if (!IsValid(CharacterOwner))
	{
		return 0.f;
	}

	if (MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking)
	{
		return 0.f;
	}

	const UCapsuleComponent* CapsuleComponent = CharacterOwner->GetCapsuleComponent();
	check(CapsuleComponent);

	const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	const FVector TraceStart(CharacterOwner->GetActorLocation());
	const FVector TranceEnd(TraceStart.X, TraceStart.Y, TraceStart.Z - CapsuleHalfHeight - GroundTraceDistance);
	const ECollisionChannel TraceChannel = UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CharacterOwner);
	FCollisionResponseParams ResponseParams;
	InitCollisionParams(QueryParams, ResponseParams);
		
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TranceEnd, TraceChannel, QueryParams, ResponseParams);

	float GroundDistance = GroundTraceDistance;
	if (HitResult.bBlockingHit)
	{
		GroundDistance = FMath::Max(0.f, HitResult.Distance - CapsuleHalfHeight);
	}
	
	return GroundDistance;
}
