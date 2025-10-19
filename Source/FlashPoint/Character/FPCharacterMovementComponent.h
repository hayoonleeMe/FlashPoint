// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPCharacterMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UFPCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFPCharacterMovementComponent();
	
	// Crouch 체크 무시하도록 오버라이드
	virtual bool CanAttemptJump() const override;

	// 캐릭터 최하단과 땅 사이의 거리를 계산해 반환한다.
	float GetGroundDistance() const;

private:
	// GetGroundDistance()에서 땅 방향으로 Trace를 수행할 때 TraceEnd의 Z 값을 결정할 값
	UPROPERTY(EditDefaultsOnly)
	float GroundTraceDistance;
};
