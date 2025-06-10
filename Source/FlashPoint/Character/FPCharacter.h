// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Data/FPAbilitySystemData.h"
#include "GameFramework/Character.h"
#include "FPCharacter.generated.h"

class UFPAttributeSet;
class UFPAbilitySystemComponent;

UCLASS()
class FLASHPOINT_API AFPCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFPCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFPAbilitySystemComponent* GetFPAbilitySystemComponent() const;
	UFPAttributeSet* GetFPAttributeSet() const;
	
protected:
	virtual void BeginPlay() override;
	
	// Initialize Gameplay Ability System
	void InitAbilitySystem();

	// DataId에 해당하는 UFPAbilitySystemData를 적용한다.
	// Server Only
	void ApplyAbilitySystemData(const FName& DataId);

	UPROPERTY()
	TObjectPtr<UFPAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFPAttributeSet> AttributeSet;

	// AbilitySystemComponent에 부여된 Ability Handle, 적용된 Effect Handle을 저장한다.
	UPROPERTY()
	FFPAbilitySystemData_GrantedHandles GrantedHandles;
};
