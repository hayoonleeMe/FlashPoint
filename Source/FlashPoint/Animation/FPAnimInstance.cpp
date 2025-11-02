// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FPGameplayTags.h"
#include "KismetAnimationLibrary.h"
#include "Character/FPCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAnimInstance)

const FVector2D UFPAnimInstance::LandRecoveryAlphaInRange = { 0.f, 0.4f };
const FVector2D UFPAnimInstance::LandRecoveryAlphaOutRange = { 0.f, 1.f };

UFPAnimInstance::UFPAnimInstance()
{
	JumpDistanceCurveName = TEXT("GroundDistance");
	DisableLeftHandIKCurveName = TEXT("DisableLeftHandIK");
	DisableRightHandIKCurveName = TEXT("DisableRightHandIK");
	CardinalDirectionDeadZone = 10.f;
	RootYawOffsetAngleClamp = { -120.f, 100.f };
	RootYawOffsetAngleCrouchClamp = { -90.f, 80.f };
	TurnYawWeightCurveName = TEXT("TurnYawWeight");
	RemainingTurnYawCurveName = TEXT("RemainingTurnYaw");

	/* FPS */
	Alpha_Spine01 = 0.15f;
	Alpha_Spine02 = 0.1f;
	Alpha_Spine03 = 0.1f;
	Alpha_Spine04 = 0.1f;
	Alpha_Spine05 = 0.1f;
	Alpha_Neck01 = 0.15f;
	Alpha_Neck02 = 0.2f;
	Alpha_Head = 0.1f;
}

void UFPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (UWeaponManageComponent* WeaponManageComp = GetOwningActor() ? GetOwningActor()->FindComponentByClass<UWeaponManageComponent>() : nullptr)
	{
		// bHasEquippedWeapon 업데이트를 위한 델레게이트 등록
		WeaponManageComp->OnEquippedWeaponChanged.AddUObject(this, &ThisClass::UpdateHasEquippedWeapon);		
	}

	// GameplayTag Property
	TagToPropertyMap.Add(FPGameplayTags::CharacterState::IsSprinting, &GameplayTag_IsSprinting);
	TagToPropertyMap.Add(FPGameplayTags::CharacterState::IsFiring, &GameplayTag_IsFiring);
	TagToPropertyMap.Add(FPGameplayTags::CharacterState::FPS, &GameplayTag_FPS);

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
	for (const auto& Pair: TagToPropertyMap)
	{
		ASC->RegisterAndCallGameplayTagEvent(Pair.Key, Delegate);
	}
}

void UFPAnimInstance::GameplayTagEventCallback(const FGameplayTag Tag, int32 NewCount)
{
	if (bool* Property = TagToPropertyMap[Tag])
	{
		*Property = NewCount > 0;
	}
}

bool UFPAnimInstance::IsFPS() const
{
	return GameplayTag_FPS && bHasEquippedWeapon;
}

void UFPAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (const ACharacter* Character = Cast<ACharacter>(GetOwningActor()))
	{
		if (const UFPCharacterMovementComponent* MoveComponent = Character->GetCharacterMovement<UFPCharacterMovementComponent>())
		{
			const FRotator Rotation = Character->GetActorRotation();
			if (!bIsFirstUpdate)
			{
				// 첫 업데이트라면 초기값인 WorldRotation과 캐릭터 Rotation의 간극이 크므로 두번째 업데이트부터 계산
				YawDeltaSinceLastUpdate = Rotation.Yaw - WorldRotation.Yaw; 
			}
			WorldRotation = Rotation;
			
			Velocity = MoveComponent->Velocity;
			GroundSpeed = Velocity.Size2D();
			DirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity, WorldRotation);
			
			bShouldMove = GroundSpeed > 0.f && MoveComponent->GetCurrentAcceleration() != FVector::ZeroVector;
			bIsOnGround = MoveComponent->IsMovingOnGround();
			bIsCrouching = Character->bIsCrouched;

			UpdateJumpData(DeltaSeconds, MoveComponent);
			UpdateAimingData(Character);
			UpdateCurrentDirection();
			UpdateBlendWeight(DeltaSeconds);
			UpdateRootYawOffset(DeltaSeconds);
			UpdateSkeletalControlData();

			if (bIsFirstUpdate)
			{
				bIsFirstUpdate = false;
			}
		}
	}
}

void UFPAnimInstance::UpdateHasEquippedWeapon(AWeapon_Base* InEquippedWeapon)
{
	EquippedWeapon = InEquippedWeapon;
	bHasEquippedWeapon = EquippedWeapon != nullptr;
	
	if (EquippedWeapon)
	{
		RightHandFPSOffset = EquippedWeapon->GetEquipInfo().RightHandFPSOffset;
	}
}

void UFPAnimInstance::UpdateJumpData(float DeltaSeconds, const UFPCharacterMovementComponent* MoveComponent)
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
	GroundDistance = MoveComponent->GetGroundDistance();
}

void UFPAnimInstance::UpdateAimingData(const ACharacter* Character)
{
	AimPitch = UKismetMathLibrary::NormalizeAxis(Character->GetBaseAimRotation().Pitch);
	FPSPitch = -AimPitch;
}

void UFPAnimInstance::UpdateCurrentDirection()
{
	const float AbsAngle = FMath::Abs(DirectionAngle);
	float FwdDeadZone = CardinalDirectionDeadZone;
	float BwdDeadZone = CardinalDirectionDeadZone;

	// Was Moving
	if (GroundSpeed > 0.f)
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
		const bool bHipFire = GameplayTag_IsFiring || (bIsOnGround && !GameplayTag_IsSprinting);
		HipFireUpperBodyBlendWeight = FMath::FInterpTo(HipFireUpperBodyBlendWeight, bHipFire ? 1.f : 0.f, DeltaSeconds, bHipFire ? 20.f : 3.f);
	}
	else
	{
		HipFireUpperBodyBlendWeight = 0.f;
	}
}

void UFPAnimInstance::UpdateSkeletalControlData()
{
	// Calc Hand IK Alpha
	if (bHasEquippedWeapon)
	{
		const float DisableLeftHandIK = GetCurveValue(DisableLeftHandIKCurveName);
		if (FMath::IsNearlyZero(DisableLeftHandIK, 1E-06))
		{
			LeftHandIKAlpha = FMath::FInterpTo(LeftHandIKAlpha, 1.f, GetDeltaSeconds(), 10.f);
		}
		else
		{
			LeftHandIKAlpha = 0.f;
		}

		const float DisableRightHandIK = GetCurveValue(DisableRightHandIKCurveName);
		if (DisableRightHandIK > 0.f)
		{
			RightHandIKAlpha = 0.f;
		}
		else
		{
			RightHandIKAlpha = FMath::FInterpTo(RightHandIKAlpha, 1.f, GetDeltaSeconds(), 8.f);
		}
	}
	else
	{
		LeftHandIKAlpha = 0.f;
		RightHandIKAlpha = 0.f;
	}

	// Calc Left Hand Attach Location
	if (LeftHandIKAlpha > 0.f)
	{
		if (EquippedWeapon)
		{
			if (const ACharacter* Character = Cast<ACharacter>(GetOwningActor()))
			{
				const FTransform LeftHandSocketTransform = EquippedWeapon->GetLeftHandSocketTransform(GameplayTag_FPS, GameplayTag_IsSprinting && bIsOnGround);
				FRotator Temp;
				Character->GetMesh()->TransformToBoneSpace(TEXT("weapon_r"), LeftHandSocketTransform.GetLocation(), LeftHandSocketTransform.Rotator(), LeftHandAttachLocation, Temp);
			}
		}
	}
}

void UFPAnimInstance::SetRootYawOffset(float InValue)
{
	RootYawOffset = UKismetMathLibrary::NormalizeAxis(InValue);

	// RootYawOffset이 과도하게 커지는(절대값 기준) 경우를 방지하기 위해 Clamp
	if (bIsCrouching)
	{
		RootYawOffset = FMath::ClampAngle(RootYawOffset, RootYawOffsetAngleCrouchClamp.X, RootYawOffsetAngleCrouchClamp.Y);
	}
	else
	{
		RootYawOffset = FMath::ClampAngle(RootYawOffset, RootYawOffsetAngleClamp.X, RootYawOffsetAngleClamp.Y);
	}

	// 실제 시점 방향으로 Aim Offset 재생
	AimYaw = -RootYawOffset;
}

void UFPAnimInstance::UpdateRootYawOffset(float DeltaSeconds)
{
	if (!bIsTurnInPlaceBlendingOut)
	{
		// Idle State인 경우, 시점 변경한 만큼 Offset 증가 (시점 방향과 반대로 증가시키기 위해 Delta를 빼줌)
		SetRootYawOffset(RootYawOffset - YawDeltaSinceLastUpdate);
	}
	else
	{
		// Idle State가 아닌 경우(Idle에서 빠져나오는 Blending Out 포함), 0으로 Interpolate해 부드럽게 캐릭터의 Root가 시점 방향과 일치하도록 회전
		SetRootYawOffset(FMath::FInterpTo(RootYawOffset, 0.f, DeltaSeconds, 10.f));
	}

	// Idle State에서 false로 설정되는 경우를 제외하곤 항상 Blending Out
	bIsTurnInPlaceBlendingOut = true;
}

void UFPAnimInstance::ProcessTurnYawCurve()
{
	// Idle State에서 호출되므로 false로 설정
	bIsTurnInPlaceBlendingOut = false;

	// 이전 프레임 값으로 업데이트
	PreviousTurnYawCurveValue = TurnYawCurveValue;

	// Turn Animation의 Curve 값을 이용해 RootYawOffset을 감소(절대값 기준)시켜 시점 방향으로 캐릭터의 Root Bone을 회전시킴
	const float TurnYawWeight = GetCurveValue(TurnYawWeightCurveName);
	if (!FMath::IsNearlyZero(TurnYawWeight))
	{
		const float RemainingTurnYaw = GetCurveValue(RemainingTurnYawCurveName);
		TurnYawCurveValue = RemainingTurnYaw / TurnYawWeight;

		if (PreviousTurnYawCurveValue != 0.f)
		{
			const float TurnYawCurveDelta = TurnYawCurveValue - PreviousTurnYawCurveValue;
			SetRootYawOffset(RootYawOffset - TurnYawCurveDelta);
		}
	}
	else
	{
		TurnYawCurveValue = 0.f;
		PreviousTurnYawCurveValue = 0.f;
	}
}
