// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FPGameplayTags.h"
#include "KismetAnimationLibrary.h"
#include "Camera/CameraComponent.h"
#include "Character/FPCharacter.h"
#include "Character/FPCharacterMovementComponent.h"
#include "Data/FPRecoilData.h"
#include "GameFramework/Character.h"
#include "Input/FPEnhancedPlayerInput.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/FPAssetManager.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAnimInstance)

const FVector2D UFPAnimInstance::LandRecoveryAlphaInRange = { 0.f, 0.4f };
const FVector2D UFPAnimInstance::LandRecoveryAlphaOutRange = { 0.f, 1.f };

UFPAnimInstance::UFPAnimInstance()
{
	JumpDistanceCurveName = TEXT("GroundDistance");
	RemoteAimPitchInterpSpeed = 15.f;
	CardinalDirectionDeadZone = 10.f;
	DisableLeftHandIKCurveName = TEXT("DisableLeftHandIK");
	LeftHandAttachDataInterpSpeed = 15.f;
	RootYawOffsetAngleClamp = { -120.f, 100.f };
	RootYawOffsetAngleCrouchClamp = { -90.f, 80.f };
	TurnYawWeightCurveName = TEXT("TurnYawWeight");
	RemainingTurnYawCurveName = TEXT("RemainingTurnYaw");
	AimDownSightAlphaInterpSpeed = 12.f;
	LookSwayAmplitude = { 0.8f, 0.8f, 0.f };
	ADSLookSwayMultiplier = 0.15f;
	MoveSwayAmplitude = { 0.8f, 0.8f, 0.f };
	JumpSwayAmplitude = 0.01f;
	MaxJumpSway = 2.f;
	ADSJumpSwayMultiplier = 0.05f;
	
	AlphaSpine3 = 0.1f;
	AlphaSpine4 = 0.1f;
	AlphaSpine5 = 0.4f;
	AlphaNeck1 = 0.15f;
	AlphaNeck2 = 0.15f;
	AlphaHead = 0.1f;
}

void UFPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// GameplayTag Property
	TagToPropertyMap.Add(FPGameplayTags::CharacterState::IsSprinting, &GameplayTag_IsSprinting);
	TagToPropertyMap.Add(FPGameplayTags::CharacterState::IsFiring, &GameplayTag_IsFiring);
	TagToPropertyMap.Add(FPGameplayTags::CharacterState::IsFirstPerson, &GameplayTag_IsFirstPerson);
	TagToPropertyMap.Add(FPGameplayTags::CharacterState::IsAimingDownSight, &GameplayTag_IsAimingDownSight);
	TagToPropertyMap.Add(FPGameplayTags::CharacterState::IsEquippingWeapon, &GameplayTag_IsEquippingWeapon);
}

void UFPAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	RegisterGameplayTagWithProperty(ASC);

	if (const APawn* Pawn = TryGetPawnOwner())
	{
		if (UWeaponManageComponent* WeaponManageComponent = FPUtils::GetComponent<UWeaponManageComponent>(Pawn))
		{
			// bHasEquippedWeapon 업데이트를 위한 델레게이트 등록
			WeaponManageComponent->OnEquippedWeaponChanged.AddUObject(this, &ThisClass::UpdateEquippedWeapon);
		
			if (Pawn->IsLocallyControlled())
			{
				// 로컬 캐릭터에서 총을 발사하고 반동을 적용할 때 Recoil Transform 계산을 위한 델레게이트 등록
				WeaponManageComponent->OnWeaponRecoilDelegate.AddUObject(this, &ThisClass::ApplyRecoil);
			}
		}
	}
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

void UFPAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (const AFPCharacter* Character = Cast<AFPCharacter>(GetOwningActor()))
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
			DirectionAngle = UKismetAnimationLibrary::CalculateDirection(MoveComponent->GetCurrentAcceleration(), WorldRotation);
			
			bShouldMove = GroundSpeed > 0.f && MoveComponent->GetCurrentAcceleration() != FVector::ZeroVector;
			bIsOnGround = MoveComponent->IsMovingOnGround();
			bIsCrouching = Character->bIsCrouched;
			
			// Weak Ptr를 각 프레임마다 한번만 Dereference하도록 캐싱 후 사용
			const AWeapon_Base* EquippedWeapon = EquippedWeaponWeakPtr.Get();

			UpdateJumpData(DeltaSeconds, MoveComponent);
			UpdateAimingData(DeltaSeconds, Character);
			UpdateCurrentDirection();
			UpdateBlendWeight(DeltaSeconds, EquippedWeapon);
			UpdateLeftHandAttachData(DeltaSeconds, EquippedWeapon);
			UpdateRootYawOffset(DeltaSeconds);
			UpdateAimDownSight(DeltaSeconds, Character, EquippedWeapon);
			UpdateFirstPersonSway(DeltaSeconds, Character);
			UpdateRecoil(DeltaSeconds);

			if (bIsFirstUpdate)
			{
				bIsFirstUpdate = false;
			}
		}
	}
}

void UFPAnimInstance::UpdateEquippedWeapon(AWeapon_Base* EquippedWeapon)
{
	EquippedWeaponWeakPtr = EquippedWeapon;
	
	if (EquippedWeaponWeakPtr.IsValid())
	{
		// Caching First Person Right Hand Offset
		EquippedWeapon->GetFirstPersonRightHandOffset(FirstPersonRightHandLocOffset, FirstPersonRightHandRotOffset);
		
		// Caching Recoil Data Asset
		CurrentRecoilData = UFPAssetManager::GetAssetByTag<UFPRecoilData>(FPGameplayTags::Asset::RecoilData, EquippedWeapon->GetWeaponTypeTag());
	}
	else
	{
		CurrentRecoilData = nullptr;
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

void UFPAnimInstance::UpdateAimingData(float DeltaSeconds, const ACharacter* Character)
{
	if (Character->IsLocallyControlled())
	{
		AimPitch = UKismetMathLibrary::NormalizeAxis(Character->GetBaseAimRotation().Pitch);
	}
	else
	{
		// SimulatedProxy에선 Character->GetBaseAimRotation().Pitch의 간극이 크므로(APawn::RemoteViewPitch를 사용하기 때문), Interpolate
		AimPitch = FMath::FInterpTo(AimPitch, UKismetMathLibrary::NormalizeAxis(Character->GetBaseAimRotation().Pitch), DeltaSeconds, RemoteAimPitchInterpSpeed);
	}
	
	FirstPersonPitch = -AimPitch;
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

void UFPAnimInstance::UpdateBlendWeight(float DeltaSeconds, const AWeapon_Base* EquippedWeapon)
{
	// Update UpperBodyAdditiveWeight
	const bool bAdditive = IsAnyMontagePlaying() && bIsOnGround;
	UpperBodyAdditiveWeight = bAdditive ? 1.f : FMath::FInterpTo(UpperBodyAdditiveWeight, 0.f, DeltaSeconds, 6.f);
	
	// Update HipFireUpperBodyBlendWeight
	if (EquippedWeapon)
	{
		const bool bHipFire = GameplayTag_IsFiring || (bIsOnGround && !GameplayTag_IsSprinting);
		HipFireUpperBodyBlendWeight = FMath::FInterpTo(HipFireUpperBodyBlendWeight, bHipFire ? 1.f : 0.f, DeltaSeconds, bHipFire ? 20.f : 3.f);
	}
	else
	{
		HipFireUpperBodyBlendWeight = 0.f;
	}
}

void UFPAnimInstance::UpdateLeftHandAttachData(float DeltaSeconds, const AWeapon_Base* EquippedWeapon)
{
	if (!EquippedWeapon)
	{
		LeftHandAttachAlpha = 0.f;
		return;
	}
	
	const float DisableLeftHandIK = GetCurveValue(DisableLeftHandIKCurveName);
	if (FMath::IsNearlyZero(DisableLeftHandIK, 1E-06))
	{
		LeftHandAttachAlpha = FMath::FInterpTo(LeftHandAttachAlpha, 1.f, DeltaSeconds, LeftHandAttachDataInterpSpeed);
	}
	else
	{
		LeftHandAttachAlpha = 0.f;
		return;
	}
	
	// Calc Relative Left Hand Attach Transform
	const FTransform WeaponAttachWorld = EquippedWeapon->GetLeftHandAttachTransform();
	GetSkelMeshComponent()->TransformToBoneSpace(TEXT("hand_r"), WeaponAttachWorld.GetLocation(), WeaponAttachWorld.Rotator(), LeftHandAttachLocation, LeftHandAttachRotation);
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

void UFPAnimInstance::UpdateAimDownSight(float DeltaSeconds, const AFPCharacter* Character, const AWeapon_Base* EquippedWeapon)
{
	if (!EquippedWeapon || !Character->IsLocallyControlled())
	{
		return;
	}
	
	// Update Alpha
	AimDownSightAlpha = Character->GetAimDownSightAlpha();
	
	// Calc ADS Transform
	USkeletalMeshComponent* SKM = GetSkelMeshComponent();
	
	// Sight를 기준으로 오른손이 얼마나 떨어져있는지를 나타내는 트랜스폼
	FTransform RightHand = SKM->GetBoneTransform(TEXT("hand_r"));
	FTransform WeaponSight = EquippedWeapon->GetAimDownSightSocketTransform();
	FTransform HandToSight = RightHand.GetRelativeTransform(WeaponSight);
	
	// Sight를 위치시킬 Target 트랜스폼
	FVector CamLoc = Character->GetFirstPersonCameraComponent()->GetComponentLocation();
	FRotator CamRot = Character->GetFirstPersonCameraComponent()->GetComponentRotation();
	FTransform CameraTarget(CamRot, CamLoc + CamRot.Vector() * 10.f);
	
	// HandToSight를 CameraTarget을 기준으로 오른손이 얼마나 떨어졌는지 나타내는 트랜스폼으로 변환
	// 즉, Sight가 CameraTarget에 있을 때 오른손이 얼마나 떨어졌는지 나타내는 트랜스폼이 계산됨
	FTransform NewHand = HandToSight * CameraTarget;
	
	// VB head_hand_r 본의 부모인 head 본 공간으로 변환 
	FVector NewLoc;
	FRotator NewRot;
	SKM->TransformToBoneSpace(TEXT("head"), NewHand.GetLocation(), NewHand.Rotator(), NewLoc, NewRot);
	
	AimDownSightLocation = FMath::VInterpTo(AimDownSightLocation, NewLoc, DeltaSeconds, AimDownSightAlphaInterpSpeed);
	AimDownSightRotation = FMath::RInterpTo(AimDownSightRotation, NewRot, DeltaSeconds, AimDownSightAlphaInterpSpeed);
}

void UFPAnimInstance::UpdateFirstPersonSway(float DeltaSeconds, const ACharacter* Character)
{
	if (!GameplayTag_IsFirstPerson)
	{
		MoveSwayAlpha = LookSwayAlpha = JumpSwayAlpha = 0.f;
		return;
	}

	// ADS일 때는 Look Sway만 적용
	MoveSwayAlpha = GameplayTag_IsAimingDownSight ? 0.f : 1.f;
	LookSwayAlpha = JumpSwayAlpha = 1.f;
	
	FVector MoveInput, LookInput;
	UFPEnhancedPlayerInput::GetMoveAndLookInputValue(Character->GetController<APlayerController>(), MoveInput, LookInput);
	
	// Move Sway
	if (MoveSwayAlpha > 0.f)
	{
		const FVector NewMoveSwayLocation = MoveInput * MoveSwayAmplitude;
		const FVector TargetMoveSwayLocation(NewMoveSwayLocation.Y, -NewMoveSwayLocation.X, 0.f);
		MoveSwayLocation = UKismetMathLibrary::VectorSpringInterp(MoveSwayLocation, TargetMoveSwayLocation, MoveSwaySpringState, 0.4f, 0.6f, DeltaSeconds, 0.006f);	
	}

	// Look Sway
	if (LookSwayAlpha > 0.f)
	{
		const FVector NewLookSwayLocation = LookInput * LookSwayAmplitude * (GameplayTag_IsAimingDownSight ? ADSLookSwayMultiplier : 1.f);
		const FVector TargetLookSwayLocation(NewLookSwayLocation.X, 0.f, -NewLookSwayLocation.Y);
		LookSwayLocation = UKismetMathLibrary::VectorSpringInterp(LookSwayLocation, TargetLookSwayLocation, LookSwaySpringState, 0.4f, 0.6f, DeltaSeconds, 0.006f);
	}
	
	// Jump Sway
	if (JumpSwayAlpha > 0.f)
	{
		const float TargetJumpSway = FMath::Clamp(Velocity.Z * JumpSwayAmplitude, -MaxJumpSway, MaxJumpSway) * (GameplayTag_IsAimingDownSight ? ADSJumpSwayMultiplier : 1.f) * -1.f;
		JumpSwayValue = UKismetMathLibrary::FloatSpringInterp(JumpSwayValue, TargetJumpSway, JumpSwaySpringState, 0.3f, 0.7f, DeltaSeconds, 0.006f);
	}
}

void UFPAnimInstance::ApplyRecoil()
{
	if (!GameplayTag_IsFirstPerson || !CurrentRecoilData)
	{
		return;
	}
	
	const float Intensity = CurrentRecoilData->AnimRecoil;
	const float UpwardMultiplier = GameplayTag_IsAimingDownSight ? CurrentRecoilData->ADSUpwardAnimRecoilMultiplier : 1.f;
	const float BackwardMultiplier = GameplayTag_IsAimingDownSight ? CurrentRecoilData->ADSBackwardAnimRecoilMultiplier : 1.f; 
	
	TargetRecoilUpward = FMath::FRandRange(-2.5f, -5.f) * Intensity * UpwardMultiplier;
	TargetRecoilBackward = FMath::FRandRange(-1.1f, -2.1f) * Intensity * BackwardMultiplier;
}

void UFPAnimInstance::UpdateRecoil(float DeltaSeconds)
{
	if (!GameplayTag_IsFirstPerson || !CurrentRecoilData)
	{
		return;
	}

	const float RecoilInterpSpeed = CurrentRecoilData->AnimRecoilInterpSpeed;
	const float RecoilRecoveryInterpSpeed = CurrentRecoilData->AnimRecoilRecoveryInterpSpeed;
	
	// Target을 향해 Interp
	RecoilUpward = FMath::FInterpTo(RecoilUpward, TargetRecoilUpward, DeltaSeconds, RecoilInterpSpeed);
	RecoilBackward = FMath::FInterpTo(RecoilBackward, TargetRecoilBackward, DeltaSeconds, RecoilInterpSpeed);
	
	// Target은 다시 0으로 Interp
	TargetRecoilUpward = FMath::FInterpTo(TargetRecoilUpward, 0.f, DeltaSeconds, RecoilRecoveryInterpSpeed);
	TargetRecoilBackward = FMath::FInterpTo(TargetRecoilBackward, 0.f, DeltaSeconds, RecoilRecoveryInterpSpeed);
}
