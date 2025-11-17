// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponLayerAnimInstance.h"

#include "FPAnimInstance.h"
#include "Component/WeaponWallTraceComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponLayerAnimInstance)

FWeaponBlockRightHandOffsetParams::FWeaponBlockRightHandOffsetParams()
{
	MinRangeOffsetByAlpha = FVector::ZeroVector;
	MaxRangeOffsetByAlpha = FVector::ZeroVector;
	MinRangeOffsetByUpPitch = FVector::ZeroVector;
	MaxRangeOffsetByUpPitch = FVector::ZeroVector;
	MinRangeOffsetByDownPitch = FVector::ZeroVector;
	MaxRangeOffsetByDownPitch = FVector::ZeroVector;
}

FVector FWeaponBlockRightHandOffsetParams::CalculateRightHandOffset(bool bIsWeaponBlocked, float WeaponBlockAlpha, float AimPitch) const
{
	FVector TargetOffset = FVector::ZeroVector;
	if (bIsWeaponBlocked)
	{
		ApplyOffsetByAlpha(TargetOffset, WeaponBlockAlpha, MinRangeOffsetByAlpha, MaxRangeOffsetByAlpha);
		if (AimPitch <= 0.f)
		{
			ApplyOffsetByDownPitch(TargetOffset, AimPitch, MinRangeOffsetByDownPitch, MaxRangeOffsetByDownPitch);
		}
		else
		{
			ApplyOffsetByUpPitch(TargetOffset, AimPitch, MinRangeOffsetByUpPitch, MaxRangeOffsetByUpPitch);
		}
	}
	return TargetOffset;
}

void FWeaponBlockRightHandOffsetParams::ApplyOffsetByAlpha(FVector& OutOffset, float WeaponBlockAlpha, const FVector& MinRangeOffset, const FVector& MaxRangeOffset)
{
	// bone space 기준이라 반대 방향
	OutOffset.X -= UKismetMathLibrary::MapRangeClamped(WeaponBlockAlpha, 0.f, 1.f, MinRangeOffset.X, MaxRangeOffset.X);
	
	OutOffset.Y += UKismetMathLibrary::MapRangeClamped(WeaponBlockAlpha, 0.f, 1.f, MinRangeOffset.Y, MaxRangeOffset.Y);
	
	OutOffset.Z += UKismetMathLibrary::MapRangeClamped(WeaponBlockAlpha, 0.f, 1.f, MinRangeOffset.Z, MaxRangeOffset.Z);
}

void FWeaponBlockRightHandOffsetParams::ApplyOffsetByUpPitch(FVector& OutOffset, float AimPitch, const FVector& MinRangeOffset, const FVector& MaxRangeOffset)
{
	// bone space 기준이라 반대 방향
	OutOffset.X -= UKismetMathLibrary::MapRangeClamped(AimPitch, 0.f, 90.f, MinRangeOffset.X, MaxRangeOffset.X);

	OutOffset.Y += UKismetMathLibrary::MapRangeClamped(AimPitch, 0.f, 90.f, MinRangeOffset.Y, MaxRangeOffset.Y);
	
	OutOffset.Z += UKismetMathLibrary::MapRangeClamped(AimPitch, 0.f, 90.f, MinRangeOffset.Z, MaxRangeOffset.Z);
}

void FWeaponBlockRightHandOffsetParams::ApplyOffsetByDownPitch(FVector& OutOffset, float AimPitch, const FVector& MinRangeOffset, const FVector& MaxRangeOffset)
{
	// bone space 기준이라 반대 방향
	OutOffset.X -= UKismetMathLibrary::MapRangeClamped(AimPitch, 0.f, -90.f, MinRangeOffset.X, MaxRangeOffset.X);

	OutOffset.Y += UKismetMathLibrary::MapRangeClamped(AimPitch, 0.f, -90.f, MinRangeOffset.Y, MaxRangeOffset.Y);
	
	OutOffset.Z += UKismetMathLibrary::MapRangeClamped(AimPitch, 0.f, -90.f, MinRangeOffset.Z, MaxRangeOffset.Z);
}

UWeaponLayerAnimInstance::UWeaponLayerAnimInstance()
{
	/* Blend Weight */
	HipFireWeightInterpSpeed = 20.f;
	HipFireWeightResetInterpSpeed = 3.f;
	
	/* Skeletal Control */
	DisableLeftHandIKCurveName = TEXT("DisableLeftHandIK");
	DisableRightHandIKCurveName = TEXT("DisableRightHandIK");
	LeftHandAttachTransformInterpSpeed = 15.f;

	/* FPS */
	Alpha_Spine03 = 0.1f;
	Alpha_Spine04 = 0.1f;
	Alpha_Spine05 = 0.4f;
	Alpha_Neck01 = 0.15f;
	Alpha_Neck02 = 0.2f;
	Alpha_Head = 0.1f;
	
	/* Weapon Block */
	WeaponBlockAlphaInRange = { 1.f, 0.5f };
	WeaponBlockAlphaInterpSpeed = 10.f;
}

void UWeaponLayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (USkeletalMeshComponent* SKM = GetOwningComponent())
	{
		MainAnimInstance = Cast<UFPAnimInstance>(SKM->GetAnimInstance());
	}
	
	if (const AActor* OwningActor = GetOwningActor())
	{
		WeaponWallTraceComponent = OwningActor->FindComponentByClass<UWeaponWallTraceComponent>();
	}
}

void UWeaponLayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (MainAnimInstance)
	{
		UpdateBlendWeight(DeltaSeconds);
		UpdateAimingData();
		UpdateSkeletalControlData(DeltaSeconds);
		UpdateWeaponBlockData(DeltaSeconds);
	}
}

void UWeaponLayerAnimInstance::UpdateBlendWeight(float DeltaSeconds)
{
	if (MainAnimInstance->bHasEquippedWeapon)
	{
		const bool bHipFire = !bIsWeaponWallBlocked && (MainAnimInstance->GameplayTag_IsFiring || (MainAnimInstance->bIsOnGround && !MainAnimInstance->GameplayTag_IsSprinting));
		HipFireUpperBodyBlendWeight = FMath::FInterpTo(HipFireUpperBodyBlendWeight, bHipFire ? 1.f : 0.f, DeltaSeconds, bHipFire ? HipFireWeightInterpSpeed : HipFireWeightResetInterpSpeed);
	}
	else
	{
		HipFireUpperBodyBlendWeight = 0.f;
	}
}

void UWeaponLayerAnimInstance::UpdateSkeletalControlData(float DeltaSeconds)
{
	// Calc Hand IK Alpha
	if (MainAnimInstance->bHasEquippedWeapon)
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

	CalculateLeftHandAttachTransform(DeltaSeconds);
}

void UWeaponLayerAnimInstance::CalculateLeftHandAttachTransform(float DeltaSeconds)
{
	if (LeftHandIKAlpha > 0.f && MainAnimInstance->EquippedWeapon)
	{
		const FTransform LeftHandSocketTransform = MainAnimInstance->EquippedWeapon->GetLeftHandSocketTransform(MainAnimInstance->GameplayTag_FPS, MainAnimInstance->GameplayTag_IsSprinting && MainAnimInstance->bIsOnGround);
		FVector NewLocation;
		FRotator NewRotation;
		GetSkelMeshComponent()->TransformToBoneSpace(TEXT("weapon_r"), LeftHandSocketTransform.GetLocation(), LeftHandSocketTransform.Rotator(), NewLocation, NewRotation);

		LeftHandAttachLocation = FMath::VInterpTo(LeftHandAttachLocation, NewLocation, DeltaSeconds, LeftHandAttachTransformInterpSpeed);
		LeftHandAttachRotation = FMath::RInterpTo(LeftHandAttachRotation, NewRotation, DeltaSeconds, LeftHandAttachTransformInterpSpeed);
	}
}

void UWeaponLayerAnimInstance::UpdateAimingData()
{
	FPSPitch = -MainAnimInstance->AimPitch;
}

const FWeaponBlockRightHandOffsetParams& UWeaponLayerAnimInstance::SelectOffsetParams() const
{
	if (MainAnimInstance->IsFPS())
	{
		return bUseWeaponUp ? FPSWeaponUpOffsetParams : FPSWeaponDownOffsetParams;
	}
	else
	{
		return bUseWeaponUp ? TPSWeaponUpOffsetParams : TPSWeaponDownOffsetParams;
	}
}

void UWeaponLayerAnimInstance::UpdateWeaponBlockData(float DeltaSeconds)
{
	APawn* Pawn = TryGetPawnOwner();
	if (Pawn && WeaponWallTraceComponent && MainAnimInstance->EquippedWeapon)
	{
		FVector LocalWallHitLocation;
		WeaponWallTraceComponent->RetrieveWeaponWallBlockData(bIsWeaponWallBlocked, bUseWeaponUp, LocalWallHitLocation);
	
		// Calc WeaponBlockAlpha
		const FVector PawnLoc = Pawn->GetActorLocation();
		float TargetAlpha = 0.f;
		if (bIsWeaponWallBlocked)
		{
			if (Pawn->IsLocallyControlled())
			{
				// AutonomousProxy 캐릭터에선, 벽까지의 거리로 벽에 근접한 비율을 계산
				const FVector WallLoc = FVector(LocalWallHitLocation.X, LocalWallHitLocation.Y, PawnLoc.Z);
				const FVector CharacterToWall = WallLoc - PawnLoc;
				// 캐릭터가 Controller Yaw를 사용하고 AnimGraph에서 AimYaw만큼 Root Bone을 회전하는데, 현재 시점에선 회전이 적용되기 전이므로 이를 적용해 전방 벡터를 구한다.
				const float ForwardYaw = UKismetMathLibrary::NormalizeAxis(Pawn->GetControlRotation().Yaw) - MainAnimInstance->AimYaw;
				const FVector ForwardVector = FRotator(0.f, ForwardYaw, 0.f).Vector();
				// 캐릭터에서 Wall Trace 충돌 지점을 향한 벡터를 전방 벡터에 투영해 벽까지의 최단 거리를 구한다.
				const float DistanceToWall = CharacterToWall.Dot(ForwardVector);
				const float NearRatio = DistanceToWall / (MainAnimInstance->EquippedWeapon->GetWallTraceLength() + 20.f);
				// 벽까지 거리에 따라 Weapon Block 적용
				TargetAlpha = FMath::GetMappedRangeValueClamped(WeaponBlockAlphaInRange, { 0.f, 1.f }, NearRatio);
			}
			else
			{
				// SimulatedProxy 캐릭터에선, 벽까지 거리에 따르지 않고 단순하게 계산
				TargetAlpha = 1.f;
			}
		}
		WeaponBlockAlpha = FMath::FInterpTo(WeaponBlockAlpha, TargetAlpha, DeltaSeconds, WeaponBlockAlphaInterpSpeed);
	
		// Calc RightHandWeaponBlockOffset
		const FVector TargetOffset = SelectOffsetParams().CalculateRightHandOffset(bIsWeaponWallBlocked, WeaponBlockAlpha, MainAnimInstance->AimPitch);
		RightHandWeaponBlockOffset = FMath::VInterpTo(RightHandWeaponBlockOffset, TargetOffset, DeltaSeconds, WeaponBlockAlphaInterpSpeed);
	}
	else
	{
		bIsWeaponWallBlocked = false;
		WeaponBlockAlpha = 0.f;
	}
}
