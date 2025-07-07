// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "FPAnimInstance.generated.h"

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

protected:
	// 초기화할 때 설정한 ASC에 등록된 GameplayTag가 추가되거나 제거되면 자동으로 등록된 프로퍼티 값이 업데이트된다.
	// Animation Blueprint에서 설정한다.
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

#if WITH_EDITOR
	// for GameplayTagPropertyMap
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
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

	// 점프 이후의 체공 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeFalling;

	// Land Recovery Additive Animation을 적용할 Alpha
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LandRecoveryAlpha;

	// 캐릭터가 이동할 방향
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECardinalDirection CurrentDirection;

	// 방향을 결정할 때 Angle에 적용할 DeadZone
	UPROPERTY(EditAnywhere)
	float CardinalDirectionDeadZone;

	void UpdateCurrentDirectionFromAngle();

	// Upper Body에 Hip Fire를 적용할지 여부를 결정하는 Blend Weight
	// 1이면 Upper Body가 총을 치켜드는 Hip Fire Pose 재생, 0이면 Base Pose 재생
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HipFireUpperBodyBlendWeight;

	void UpdateBlendWeight(float DeltaSeconds);
};
