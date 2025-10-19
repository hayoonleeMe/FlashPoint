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
	CardinalDirectionDeadZone = 10.f;
	RootYawOffsetAngleClamp = { -120.f, 100.f };
	RootYawOffsetAngleCrouchClamp = { -90.f, 80.f };
	TurnYawWeightCurveName = TEXT("TurnYawWeight");
	RemainingTurnYawCurveName = TEXT("RemainingTurnYaw");
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
	ASC->RegisterAndCallGameplayTagEvent(FPGameplayTags::CharacterState::IsSprinting, Delegate);
	ASC->RegisterAndCallGameplayTagEvent(FPGameplayTags::CharacterState::IsFiring, Delegate);
}

void UFPAnimInstance::GameplayTagEventCallback(const FGameplayTag Tag, int32 NewCount)
{
	if (bool* Property = TagToPropertyMap[Tag])
	{
		*Property = NewCount > 0;
	}
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

			if (bIsFirstUpdate)
			{
				bIsFirstUpdate = false;
			}
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
		const bool bHipFire = GameplayTag_IsFiring || (bIsOnGround && !GameplayTag_IsSprinting);
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
