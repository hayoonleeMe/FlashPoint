// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "FPAnimInstance.generated.h"

class AWeapon_Base;
class UCharacterMovementComponent;
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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
	
protected:
	// GameplayTag에 해당하는 프로퍼티 값을 업데이트하도록 등록
	void RegisterGameplayTagWithProperty(UAbilitySystemComponent* ASC);
	
	// GameplayTag에 해당하는 프로퍼티 값을 업데이트
	void GameplayTagEventCallback(const FGameplayTag Tag, int32 NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool GameplayTag_IsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool GameplayTag_IsFiring;

	TMap<FGameplayTag, bool*> TagToPropertyMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Velocity;
	
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

	void UpdateHasEquippedWeapon(AWeapon_Base* EquippedWeapon);

	// 무기를 발사하고 난 뒤, 지난 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeSinceLastFire;

	// 무기를 발사하고 난 뒤, 무기를 들지 않고 다시 원래 동작을 수행할 때까지 기다릴 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AimHoldDuration;

	// 점프 이후의 체공 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeFalling;

	// Land Recovery Additive Animation을 적용할 Alpha
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LandRecoveryAlpha;

	// LandRecoveryAlpha 값을 Clamp 할 때 사용할 Range
	const static FVector2D LandRecoveryAlphaInRange;
	const static FVector2D LandRecoveryAlphaOutRange;

	void UpdateJumpData(float DeltaSeconds, const UCharacterMovementComponent* MoveComponent);

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
};
