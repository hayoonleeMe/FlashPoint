// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FPGameplayTags.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAnimInstance)

const FVector2D UFPAnimInstance::LandRecoveryAlphaInRange = { 0.f, 0.4f };
const FVector2D UFPAnimInstance::LandRecoveryAlphaOutRange = { 0.f, 1.f };

UFPAnimInstance::UFPAnimInstance()
{
	CardinalDirectionDeadZone = 10.f;
	AimHoldDuration = 1.5f;
}

void UFPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 0으로 시작해 Blend Weight를 업데이트 하지 않도록 방지
	TimeSinceLastFire = AimHoldDuration;

	if (UWeaponManageComponent* WeaponManageComp = GetOwningActor() ? GetOwningActor()->FindComponentByClass<UWeaponManageComponent>() : nullptr)
	{
		// bHasEquippedWeapon 업데이트를 위한 델레게이트 등록
		WeaponManageComp->OnEquippedWeaponChanged.AddUObject(this, &ThisClass::UpdateHasEquippedWeapon);		
	}

	// GameplayTag Property
	TagToPropertyMap.Add(FPGameplayTags::CharacterState_IsSprinting, &GameplayTag_IsSprinting);
	TagToPropertyMap.Add(FPGameplayTags::CharacterState_IsFiring, &GameplayTag_IsFiring);

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningActor()))
	{
		RegisterGameplayTagWithProperty(ASC);
	}
}

void UFPAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	RegisterGameplayTagWithProperty(ASC);
}

void UFPAnimInstance::RegisterGameplayTagWithProperty(UAbilitySystemComponent* ASC)
{
	check(ASC);

	FOnGameplayEffectTagCountChanged::FDelegate Delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ThisClass::GameplayTagEventCallback);
	ASC->RegisterAndCallGameplayTagEvent(FPGameplayTags::CharacterState_IsSprinting, Delegate);
	ASC->RegisterAndCallGameplayTagEvent(FPGameplayTags::CharacterState_IsFiring, Delegate);
}

void UFPAnimInstance::GameplayTagEventCallback(const FGameplayTag Tag, int32 NewCount)
{
	if (bool* Property = TagToPropertyMap[Tag])
	{
		*Property = NewCount > 0;
	}
}

void UFPAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ACharacter* Character = Cast<ACharacter>(GetOwningActor()))
	{
		if (const UCharacterMovementComponent* MoveComponent = Character->GetCharacterMovement())
		{
			Velocity = MoveComponent->Velocity;
			GroundSpeed = Velocity.Size2D();
			DirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
			
			bShouldMove = GroundSpeed > 0.f && MoveComponent->GetCurrentAcceleration() != FVector::ZeroVector;
			bIsOnGround = MoveComponent->IsMovingOnGround();
			bIsCrouching = Character->bIsCrouched;

			if (GameplayTag_IsFiring)
			{
				TimeSinceLastFire = 0.f;
			}
			else if (TimeSinceLastFire < AimHoldDuration)
			{
				TimeSinceLastFire += DeltaSeconds;
			}

			UpdateJumpData(DeltaSeconds, MoveComponent);
			UpdateAimingData(Character);
			UpdateCurrentDirection();
			UpdateBlendWeight(DeltaSeconds);
		}
	}
}

void UFPAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();

	// NativeUpdateAnimation()에서 CurveValue를 가져오면 처음 약간의 프레임 동안 제대로 가져오지 못함
	// 따라서 평가가 끝난 후 CurveValue를 제대로 가져올 수 있을 때 업데이트
	UpdateLeftHandModifyTransform();
}

void UFPAnimInstance::UpdateHasEquippedWeapon(AWeapon_Base* EquippedWeapon)
{
	EquippedWeaponWeakPtr = EquippedWeapon;
	bHasEquippedWeapon = EquippedWeapon != nullptr;
}

void UFPAnimInstance::UpdateJumpData(float DeltaSeconds, const UCharacterMovementComponent* MoveComponent)
{
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
	
	LandRecoveryAlpha = FMath::GetMappedRangeValueClamped(LandRecoveryAlphaInRange, LandRecoveryAlphaOutRange, TimeFalling);
}

void UFPAnimInstance::UpdateAimingData(const ACharacter* Character)
{
	AimPitch = UKismetMathLibrary::NormalizeAxis(Character->GetBaseAimRotation().Pitch);
}

void UFPAnimInstance::UpdateCurrentDirection()
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
	// Update UpperBodyAdditiveWeight
	const bool bAdditive = IsAnyMontagePlaying() && bIsOnGround;
	UpperBodyAdditiveWeight = bAdditive ? 1.f : FMath::FInterpTo(UpperBodyAdditiveWeight, 0.f, DeltaSeconds, 6.f);
	
	// Update HipFireUpperBodyBlendWeight
	if (bHasEquippedWeapon)
	{
		const bool bHipFire = GameplayTag_IsFiring || TimeSinceLastFire < AimHoldDuration || (bIsOnGround && !GameplayTag_IsSprinting);
		HipFireUpperBodyBlendWeight = FMath::FInterpTo(HipFireUpperBodyBlendWeight, bHipFire ? 1.f : 0.f, DeltaSeconds, bHipFire ? 20.f : 3.f);
	}
	else
	{
		HipFireUpperBodyBlendWeight = 0.f;
	}
}

void UFPAnimInstance::UpdateLeftHandModifyTransform()
{
	if (EquippedWeaponWeakPtr.IsValid())
	{
		if (ACharacter* Character = Cast<ACharacter>(GetOwningActor()))
		{
			if (Character && Character->GetMesh())
			{
				float DisableLeftHandIK = GetCurveValue(TEXT("DisableLeftHandIK"));
				if (DisableLeftHandIK > 0.f)
				{
					LeftHandModifyAlpha = 0.f;
				}
				else
				{
					LeftHandModifyAlpha = FMath::FInterpTo(LeftHandModifyAlpha, 1.f, GetDeltaSeconds(), 8.f);
				}
			
				const FTransform WeaponAttachTransform = EquippedWeaponWeakPtr->GetLeftHandAttachTransform();
				FVector MeshAttachLocation;
				FRotator Temp;
				Character->GetMesh()->TransformToBoneSpace(TEXT("hand_r"), WeaponAttachTransform.GetLocation(), WeaponAttachTransform.Rotator(), MeshAttachLocation, Temp);

				LeftHandModifyTransform = FTransform(MeshAttachLocation);
			}	
		}
	}
	else
	{
		LeftHandModifyAlpha = 0.f;
	}
}
