// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponLayerAnimInstance.generated.h"

class UFPAnimInstance;
class UWeaponWallTraceComponent;

/**
 * Weapon Block 상태에서의 오른손 오프셋 계산을 수행하고 관련 데이터를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FWeaponBlockRightHandOffsetParams
{
	GENERATED_BODY()

public:
	FWeaponBlockRightHandOffsetParams();
	
	// 상황에 맞는 Right Hand Offset을 계산해 반환한다.
	FVector CalculateRightHandOffset(bool bIsWeaponBlocked, float WeaponBlockAlpha, float AimPitch) const;

private:
	static void ApplyOffsetByAlpha(FVector& OutOffset, float WeaponBlockAlpha, const FVector& MinRangeOffset, const FVector& MaxRangeOffset);
	static void ApplyOffsetByUpPitch(FVector& OutOffset, float AimPitch, const FVector& MinRangeOffset, const FVector& MaxRangeOffset);
	static void ApplyOffsetByDownPitch(FVector& OutOffset, float AimPitch, const FVector& MinRangeOffset, const FVector& MaxRangeOffset);

	// WeaponBlockAlpha가 0일 때 적용할 오프셋
	UPROPERTY(EditAnywhere)
	FVector MinRangeOffsetByAlpha;

	// WeaponBlockAlpha가 1일 때 적용할 오프셋
	UPROPERTY(EditAnywhere)
	FVector MaxRangeOffsetByAlpha;

	// AimPitch가 0 초과일 때, AimPitch가 0일 때 적용할 오프셋
	UPROPERTY(EditAnywhere)
	FVector MinRangeOffsetByUpPitch;

	// AimPitch가 0 초과일 때, AimPitch가 90일 때 적용할 오프셋
	UPROPERTY(EditAnywhere)
	FVector MaxRangeOffsetByUpPitch;

	// AimPitch가 0 이하일 때, AimPitch가 0일 때 적용할 오프셋
	UPROPERTY(EditAnywhere)
	FVector MinRangeOffsetByDownPitch;

	// AimPitch가 0 이하일 때, AimPitch가 -90일 때 적용할 오프셋
	UPROPERTY(EditAnywhere)
	FVector MaxRangeOffsetByDownPitch;
};

/**
 * ALI_WeaponLayers 인터페이스를 구현하는 Anim Instance
 *
 * 실제 설정과 구현은 ABP_WeaponLayers 애셋에서 수행하고, 이 클래스는 프로퍼티 선언과 업데이트를 위한 Base 클래스이다.
 */
UCLASS()
class FLASHPOINT_API UWeaponLayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UWeaponLayerAnimInstance();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	void RetrieveWeaponReloadData(bool& bOutIsReloading, float& OutWeaponBlockAlpha, bool& bOutUseWeaponUp) const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFPAnimInstance> MainAnimInstance;

	// ============================================================================
	// Blend Weight 
	// ============================================================================

	// Upper Body에 Hip Fire를 적용할지 여부를 결정하는 Blend Weight
	// 1이면 Upper Body가 총을 치켜드는 Hip Fire Pose 재생, 0이면 Base Pose 재생
	// 추가로, 1이면 Idle Aim Offset, 0이면 Relaxed Aim Offset 재생
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blend Weight")
	float HipFireUpperBodyBlendWeight;

	UPROPERTY(EditAnywhere, Category="Blend Weight")
	float HipFireWeightInterpSpeed;

	UPROPERTY(EditAnywhere, Category="Blend Weight")
	float HipFireWeightResetInterpSpeed;

	void UpdateBlendWeight(float DeltaSeconds);

	// ============================================================================
	// Skeletal Control
	// ============================================================================
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skeletal Control")
    FName DisableLeftHandIKCurveName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skeletal Control")
    float LeftHandIKAlpha;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skeletal Control")
    FName DisableRightHandIKCurveName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skeletal Control")
    float RightHandIKAlpha;

    void UpdateSkeletalControlData(float DeltaSeconds);
	
	// 왼손을 무기에 부착할 위치
	// Character Mesh의 weapon_r bone 기준
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skeletal Control")
	FVector LeftHandAttachLocation;

	// 왼손을 무기에 부착할 회전값
	// Character Mesh의 weapon_r bone 기준
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skeletal Control")
	FRotator LeftHandAttachRotation;

	UPROPERTY(EditAnywhere, Category="Skeletal Control")
	float LeftHandAttachTransformInterpSpeed;

    void CalculateLeftHandAttachTransform(float DeltaSeconds);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skeletal Control")
	FVector LeftHandAttachJointTargetLocation;

	UPROPERTY(EditAnywhere, Category="Skeletal Control")
	FVector LeftHandAttachNormalJoint;

	UPROPERTY(EditAnywhere, Category="Skeletal Control")
	FVector LeftHandAttachWeaponBlockUpJoint;
	
	UPROPERTY(EditAnywhere, Category="Skeletal Control")
	FVector LeftHandAttachWeaponBlockDownJoint;

	UPROPERTY(EditAnywhere, Category="Skeletal Control")
	float LeftHandAttachJointInterpSpeed;

	// ============================================================================
	// FPS
	// ============================================================================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS")
	float Alpha_Spine03;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS")
	float Alpha_Spine04;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS")
	float Alpha_Spine05;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS")
	float Alpha_Neck01;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS")
	float Alpha_Neck02;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS")
	float Alpha_Head;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS")
	float FPSPitch;

	void UpdateAimingData();
	
	// ============================================================================
	// Weapon Block
	// ============================================================================

	// WeaponBlockAlpha 계산 시, 적용할 벽까지의 거리 비율 범위
	UPROPERTY(EditAnywhere, Category="Weapon Block")
	FVector2D WeaponBlockAlphaInRange;

	// 캐릭터가 벽에 근접해 무기가 Block 됐는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Block")
	bool bIsWeaponWallBlocked;

	// Weapon Block 시 Up Pose를 사용할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Block")
	bool bUseWeaponUp;

	UPROPERTY(EditAnywhere, Category="Weapon Block")
	float WeaponBlockAlphaInterpSpeed;
	
	// Weapon Block Anim을 블렌드할 Alpha
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Block")
	float WeaponBlockAlpha;

	// Weapon Block Anim을 블렌드할 Alpha
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Block")
	float WeaponBlockRotationAlpha;

	// Weapon Block을 수행할 때 오른손에 적용할 Location
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Block")
	FVector RightHandWeaponBlockLocationOffset;

	// Weapon Block을 수행할 때 오른손에 적용할 Rotation Offset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Block")
	FRotator RightHandWeaponBlockRotationOffset;

	// TPS 시점에서 Weapon Block Up 상태일 때의 Offset Params
	UPROPERTY(EditAnywhere, Category="Weapon Block")
	FWeaponBlockRightHandOffsetParams TPSWeaponUpOffsetParams;

	// TPS 시점에서 Weapon Block Down 상태일 때의 Offset Params
	UPROPERTY(EditAnywhere, Category="Weapon Block")
	FWeaponBlockRightHandOffsetParams TPSWeaponDownOffsetParams;

	// FPS 시점에서 Weapon Block Up 상태일 때의 Offset Params
	UPROPERTY(EditAnywhere, Category="Weapon Block")
	FWeaponBlockRightHandOffsetParams FPSWeaponUpOffsetParams;

	// FPS 시점에서 Weapon Block Down 상태일 때의 Offset Params
	UPROPERTY(EditAnywhere, Category="Weapon Block")
	FWeaponBlockRightHandOffsetParams FPSWeaponDownOffsetParams;

	// 현재 상황에 맞는 Offset Params 객체를 선택해 반환한다.
	const FWeaponBlockRightHandOffsetParams& SelectOffsetParams() const;

	UPROPERTY()
	TObjectPtr<UWeaponWallTraceComponent> WeaponWallTraceComponent;
	
	void UpdateWeaponBlockData(float DeltaSeconds);
};
