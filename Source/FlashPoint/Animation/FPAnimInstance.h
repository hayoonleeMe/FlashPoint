// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "FPAnimInstance.generated.h"

class UFPCharacterMovementComponent;
class AWeapon_Base;
class UAbilitySystemComponent;

/**
 * 캐릭터의 방향별 움직임 애니메이션을 결정하는 enum 
 */
UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	Forward,
	Backward,
	Right,
	Left
};

/**
 * Default Player Character Anim Instance
 */
UCLASS()
class FLASHPOINT_API UFPAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFPAnimInstance();
	virtual void NativeInitializeAnimation() override;
	virtual void NativePostEvaluateAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	
	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
	
protected:
	// GameplayTag에 해당하는 프로퍼티 값을 업데이트하도록 등록
	void RegisterGameplayTagWithProperty(UAbilitySystemComponent* ASC);
	
	// GameplayTag에 해당하는 프로퍼티 값을 업데이트
	void GameplayTagEventCallback(const FGameplayTag Tag, int32 NewCount);

	bool bIsFirstUpdate = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool GameplayTag_IsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool GameplayTag_IsFiring;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool GameplayTag_FPS;

	TMap<FGameplayTag, bool*> TagToPropertyMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float YawDeltaSinceLastUpdate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator WorldRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DirectionAngle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsOnGround : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsCrouching : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bHasEquippedWeapon : 1;

	UPROPERTY()
	TWeakObjectPtr<AWeapon_Base> EquippedWeaponWeakPtr;

	void UpdateHasEquippedWeapon(AWeapon_Base* EquippedWeapon);

	// 캐릭터 최하단에서 땅까지 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GroundDistance;

	// Fall Land Anim의 Distance Curve 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName JumpDistanceCurveName;
	
	// 점프 이후의 체공 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeFalling;

	// Land Recovery Additive Animation을 적용할 Alpha
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LandRecoveryAlpha;

	// LandRecoveryAlpha 값을 Clamp 할 때 사용할 Range
	const static FVector2D LandRecoveryAlphaInRange;
	const static FVector2D LandRecoveryAlphaOutRange;

	void UpdateJumpData(float DeltaSeconds, const UFPCharacterMovementComponent* MoveComponent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimYaw;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimPitch;

	void UpdateAimingData(const ACharacter* Character);

	// 캐릭터가 이동할 방향
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECardinalDirection CurrentDirection;

	// 방향을 결정할 때 Angle에 적용할 DeadZone
	UPROPERTY(EditAnywhere)
	float CardinalDirectionDeadZone;

	void UpdateCurrentDirection();

	// Upper Body Additive Slot Anim Montage를 Locomotion Pose에 Apply Additive 할 때 사용할 Alpha
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UpperBodyAdditiveWeight;

	// Upper Body에 Hip Fire를 적용할지 여부를 결정하는 Blend Weight
	// 1이면 Upper Body가 총을 치켜드는 Hip Fire Pose 재생, 0이면 Base Pose 재생
	// 추가로, 1이면 Idle Aim Offset, 0이면 Relaxed Aim Offset 재생
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HipFireUpperBodyBlendWeight;

	void UpdateBlendWeight(float DeltaSeconds);

	// 왼손을 무기에 부착할 Transform (Only Translation)
	// Character Mesh의 hand_r 기준
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform LeftHandModifyTransform;

	// 왼손을 무기에 부착할지를 결정하는 Alpha
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LeftHandModifyAlpha;

	void UpdateLeftHandModifyTransform();

	// TurnInPlace가 Blend Out 상태인지 여부
	// Idle State 일 때 false로 설정되고, 그 외에는 true이다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bIsTurnInPlaceBlendingOut : 1;

	// Anim Graph의 Rotate Root Bone 노드의 Yaw 값으로 사용되고, Idle 상태일 때 bUseControllerRotationYaw 옵션을 사용하는 캐릭터가 시점 방향으로 회전하지 않도록 역방향으로 회전시키는 역할을 한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RootYawOffset;

	// RootYawOffset 값을 설정할 때 Clamp 설정
	// X: Min, Y: Max
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D RootYawOffsetAngleClamp;

	// Crouch 상태에서 RootYawOffset 값을 설정할 때 Clamp 설정
	// X: Min, Y: Max
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D RootYawOffsetAngleCrouchClamp;

	// RootYawOffset을 보정한 뒤 InValue로 설정한다.
	void SetRootYawOffset(float InValue);

	// Turn Animation의 TurnYawWeight Curve Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TurnYawWeightCurveName;

	// Turn Animation의 RemainingTurnYaw Curve Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RemainingTurnYawCurveName;

	// Turn Animation의 Turn Yaw Curve Value 들로 계산된 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnYawCurveValue;

	// 이전 프레임에 설정된 TurnYawCurveValue
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PreviousTurnYawCurveValue;

	void UpdateRootYawOffset(float DeltaSeconds);

	// 현재 애니메이션의 TurnYawWeightCurveName, RemainingTurnYawCurveName Curve 값에 따라 RootYawOffset을 업데이트한다.
	// Idle State일 때 호출된다. (Idle에서 Blending Out 될 때 제외)
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	void ProcessTurnYawCurve();
};
