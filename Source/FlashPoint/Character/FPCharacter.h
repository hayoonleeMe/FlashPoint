﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Data/FPAbilitySystemData.h"
#include "GameFramework/Character.h"
#include "FPCharacter.generated.h"

class UWeaponManageComponent;
class UCameraComponent;
class USpringArmComponent;
class UFPAttributeSet;
class UFPAbilitySystemComponent;

UCLASS()
class FLASHPOINT_API AFPCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFPCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFPAbilitySystemComponent* GetFPAbilitySystemComponent() const;
	UFPAttributeSet* GetFPAttributeSet() const;

	// 입력을 통해 캐릭터가 움직이는지 반환
	bool IsMovingFromInput() const;

	// Crouch 체크 무시하도록 오버라이드
	virtual bool CanJumpInternal_Implementation() const override;
	
protected:
	virtual void BeginPlay() override;

	// ============================================================================
	// Ability System
	// ============================================================================
	
	// Initialize Gameplay Ability System
	void InitAbilitySystem();

	UPROPERTY()
	TObjectPtr<UFPAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFPAttributeSet> AttributeSet;

	// AbilitySystemComponent에 부여된 Ability Handle, 적용된 Effect Handle을 저장한다.
	UPROPERTY()
	FFPAbilitySystemData_GrantedHandles GrantedHandles;

public:
	// DataId에 해당하는 UFPAbilitySystemData를 ASC에 적용한다.
	// Server Only
	void ApplyAbilitySystemData(const FName& DataId);

	// DataTag에 해당하는 UFPAbilitySystemData를 ASC에 적용한다.
	// Server Only
	void ApplyAbilitySystemData(const FGameplayTag& DataTag);

	// DataId에 해당하는 UFPAbilitySystemData를 ASC에서 제거한다.
	// Server Only
	void RemoveAbilitySystemData(const FName& DataId);

	// DataId에 해당하는 UFPAbilitySystemData를 ASC에서 제거한다.
	// Server Only
	void RemoveAbilitySystemData(const FGameplayTag& DataTag);

protected:
	// ============================================================================
	// Camera
	// ============================================================================
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	// 실제로 Spring Arm Component의 Z Location을 설정하는 데 사용
	float CurrentCameraHeight = 0.f;

	// CurrentCameraHeight를 Interpolation을 통해 도달할 Target
	float TargetCameraHeight = 0.f;

	// ============================================================================
	// Weapon
	// ============================================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)	// TODO : BlueprintReadOnly For Test
	TObjectPtr<UWeaponManageComponent> WeaponManageComponent;
};
