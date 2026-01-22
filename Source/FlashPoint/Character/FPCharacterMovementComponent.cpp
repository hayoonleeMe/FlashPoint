// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCharacterMovementComponent)

UFPCharacterMovementComponent::UFPCharacterMovementComponent()
{
	GroundTraceDistance = 100000.f;
	
	SprintSpeedMultiplier = 1.5f;
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

void UFPCharacterMovementComponent::FFPSavedMove::Clear()
{
	Super::Clear();
	
	bWantsToSprint = false;
}

void UFPCharacterMovementComponent::FFPSavedMove::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	
	if (UFPCharacterMovementComponent* CharacterMovement = C->GetCharacterMovement<UFPCharacterMovementComponent>())
	{
		bWantsToSprint = CharacterMovement->bWantsToSprint;
	}
}

bool UFPCharacterMovementComponent::FFPSavedMove::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const
{
	const FFPSavedMove* NewMove = static_cast<FFPSavedMove*>(NewMovePtr.Get());
	
	if (bWantsToSprint != NewMove->bWantsToSprint)
	{
		return false;
	}
	
	return Super::CanCombineWith(NewMovePtr, InCharacter, MaxDelta);
}

uint8 UFPCharacterMovementComponent::FFPSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags(); 
	
	if (bWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}
	
	return Result;
}

UFPCharacterMovementComponent::FFPNetworkPredictionData_Client::FFPNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UFPCharacterMovementComponent::FFPNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FFPSavedMove());
}

void UFPCharacterMovementComponent::StartSprint()
{
	bWantsToSprint = true;
}

void UFPCharacterMovementComponent::StopSprint()
{
	bWantsToSprint = false;
}

float UFPCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();
	
	if (bWantsToSprint)
	{
		MaxSpeed *= SprintSpeedMultiplier;
	}
	
	return MaxSpeed;
}

FNetworkPredictionData_Client* UFPCharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UFPCharacterMovementComponent* MutableThis = const_cast<UFPCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FFPNetworkPredictionData_Client(*this);
	}
	return ClientPredictionData;
}

void UFPCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	// FSavedMove_Client에 압축되어 저장된 입력 Flag를 Character Movement Component로 가져온다.
	// 이를 통해 이동 속도를 결정할 상태를 업데이트한다.
	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}
