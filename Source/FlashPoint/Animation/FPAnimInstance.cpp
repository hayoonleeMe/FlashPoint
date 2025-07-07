// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAnimInstance)

UFPAnimInstance::UFPAnimInstance()
{
	CardinalDirectionDeadZone = 10.f;
}

void UFPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningActor()))
	{
		GameplayTagPropertyMap.Initialize(this, ASC);
	}
}

void UFPAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const ACharacter* Character = Cast<ACharacter>(GetOwningActor()))
	{
		if (const UCharacterMovementComponent* MoveComponent = Character->GetCharacterMovement())
		{
			Velocity = MoveComponent->Velocity;
			GroundSpeed = Velocity.Size2D();
			DirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
			
			bShouldMove = GroundSpeed > 0.f && MoveComponent->GetCurrentAcceleration() != FVector::ZeroVector;
			bIsOnGround = MoveComponent->IsMovingOnGround();
			bIsCrouching = Character->bIsCrouched;

			bIsJumping = bIsFalling = false;
			if (MoveComponent->IsFalling())
			{
				if (Velocity.Z > 0.f)
				{
					bIsJumping = true;
					TimeFalling = 0.f;
				}
				else
				{
					bIsFalling = true;
					TimeFalling += DeltaSeconds;
				}
			}

			UpdateCurrentDirectionFromAngle();
			UpdateBlendWeight(DeltaSeconds);
		}	
	}
}

#if WITH_EDITOR
EDataValidationResult UFPAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return Context.GetNumErrors() ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif

void UFPAnimInstance::UpdateCurrentDirectionFromAngle()
{
	const float AbsAngle = FMath::Abs(DirectionAngle);
	float FwdDeadZone = CardinalDirectionDeadZone;
	float BwdDeadZone = CardinalDirectionDeadZone;

	// Was Moving
	if (Velocity.Size2D() > 0.f)
	{
		// 앞 또는 뒤로 이동 중이면 진행 방향을 벗어나기 힘들도록 DeadZone을 키움
		switch (CurrentDirection)
		{
		case ECardinalDirection::Forward:
			FwdDeadZone *= 2.f;
			break;
		case ECardinalDirection::Backward:
			BwdDeadZone *= 2.f;
			break;
		default:
			break;
		}
	}

	if (AbsAngle <= FwdDeadZone + 45.f)
	{
		CurrentDirection = ECardinalDirection::Forward;
	}
	else if (AbsAngle >= 135.f - BwdDeadZone)
	{
		CurrentDirection = ECardinalDirection::Backward;
	}
	else if (DirectionAngle > 0.f)
	{
		CurrentDirection = ECardinalDirection::Right;
	}
	else
	{
		CurrentDirection = ECardinalDirection::Left;
	}
}

void UFPAnimInstance::UpdateBlendWeight(float DeltaSeconds)
{
	// TODO
}
