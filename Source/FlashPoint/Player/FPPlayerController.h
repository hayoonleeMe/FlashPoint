﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "FPPlayerController.generated.h"

class UFPAbilitySystemComponent;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class FLASHPOINT_API AFPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFPPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void Input_Move(const FInputActionValue& InputValue);
	void Input_Look(const FInputActionValue& InputValue);
	void Input_Crouch();

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	UFPAbilitySystemComponent* GetFPAbilitySystemComponent() const;
};
