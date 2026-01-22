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

	// ============================================================================
	// 로컬 클라이언트에서 CMC(Character Movement Component) 상태를 SetMoveFor()로 저장한 SavedMove 이동 데이터를 GetCompressedFlags()로 압축해 서버로 전송 
	// 서버에서 압축된 데이터를 받아 CMC에서 UpdateFromCompressedFlags()로 압축 해제 후 상태 업데이트
	// CMC에서 상태에 따라 이동속도 변경

	class FFPSavedMove : public FSavedMove_Character
	{
		using Super = FSavedMove_Character;
		
	public:
		// Sprint를 수행하는지 여부를 나타내는 상태 (FLAG_Custom_0 플래그 사용)
		uint8 bWantsToSprint : 1;
		
		// Move 데이터 초기화
		virtual void Clear() override;

		// Character Movement Component 데이터로 Move 데이터 초기화
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		
		// 두 Move 데이터를 하나로 합칠지 결정 (새 Move 데이터와 상태 데이터가 다른지 체크)
		virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;
		
		// 상태를 플래그로 압축
		virtual uint8 GetCompressedFlags() const override;
	};

	class FFPNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
		using Super = FNetworkPredictionData_Client_Character;
		
	public:
		FFPNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		// 커스텀 FFPSavedMove 클래스를 사용하기 위한 Boilerplate
		virtual FSavedMovePtr AllocateNewMove() override;
	};
	
public:
	void StartSprint();
	void StopSprint();
	
	uint8 bWantsToSprint : 1;
	
	virtual float GetMaxSpeed() const override;
	
	// 커스텀 FFPNetworkPredictionData_Client 클래스를 사용하기 위한 Boilerplate
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
protected:
	// SavedMove로 받은 압축된 플래그 값으로 상태 업데이트
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Speed")
	float SprintSpeedMultiplier;
};
