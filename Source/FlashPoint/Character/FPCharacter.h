// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Data/FPAbilitySystemData.h"
#include "GameFramework/Character.h"
#include "FPCharacter.generated.h"

class UFPCameraOffset;
class UFPCameraComponent;
class UCustomFovComponent;
class UWeaponManageComponent;
class UCameraComponent;
class USpringArmComponent;
class UFPAttributeSet;
class UFPAbilitySystemComponent;

/**
 * Gameplay에서 사용할 Character
 */
UCLASS()
class FLASHPOINT_API AFPCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFPCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void PawnClientRestart() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFPAbilitySystemComponent* GetFPAbilitySystemComponent() const;
	UFPAttributeSet* GetFPAttributeSet() const;

	// 입력을 통해 캐릭터가 전방으로 움직이는지 반환
	bool IsMovingForwardFromInput() const;

	// Crouch 체크 무시하도록 오버라이드
	virtual bool CanJumpInternal_Implementation() const override;
	
protected:
	// 캐릭터 메시를 설정한다.
	void SetCharacterMesh();

	// 이전 프레임에서 캐릭터가 입력에 의해 전방으로 움직였는지
	bool bWasMovingForwardFromInput = false;

	// ============================================================================
	// Ability System
	// ============================================================================
	
	// Initialize Gameplay Ability System
	void InitAbilitySystem();

	UPROPERTY()
	TObjectPtr<UFPAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFPAttributeSet> AttributeSet;

protected:
	// ============================================================================
	// Camera
	// ============================================================================
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	// 3인칭 카메라
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;

	// 실제로 Spring Arm Component의 Z Location을 설정하는 데 사용
	float CurrentCameraHeight = 0.f;

	// CurrentCameraHeight를 Interpolation을 통해 도달할 Target
	float TargetCameraHeight = 0.f;
	
	// 1인칭 카메라
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFPCameraComponent> FirstPersonCameraComponent;
	
	// First Person Camera에 적용할 Camera Offset을 결정한다.
	TSubclassOf<UFPCameraOffset> DetermineCameraOffset() const;
	
public:
	// 카메라를 전환한다. (1인칭 - 3인칭)
	void ToggleCamera() const;
	
	UCameraComponent* GetFirstPersonCameraComponent() const;

	// ============================================================================
	// Weapon
	// ============================================================================

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeaponManageComponent> WeaponManageComponent;

	// 카메라의 기본 FOV
	// todo : 게임 설정과 연동해 초기화
	float BaseFOV = 0.f;

	// ADS 진행 정도를 나타내는 Alpha
	float AimDownSightAlpha = 0.f;
	bool bAimDownSightStarted = false;
	float CachedAimDownSightFOV = 0.f;
	float CachedTimeToADS = 0.f;
	
	// ADS를 수행하기 전 3인칭 시점이었는지
	bool bIsThirdPersonBeforeADS = false;
	
	void UpdateAimDownSight(float DeltaSeconds);
	
public:
	void StartAimDownSight(float CameraFOV, float WeaponFOV, float TimeToADS);
	void StopAimDownSight(float WeaponFOV);
	
	float GetAimDownSightAlpha() const { return AimDownSightAlpha; }
	
	// ADS가 완전히 끝나 다시 시작할 수 있을지 여부를 반환
	bool CanStartAimDownSight() const;

	// ============================================================================
	// CustomFOV
	// ============================================================================
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="CustomFOV")
	TObjectPtr<UCustomFovComponent> CustomFovComponent;
};
