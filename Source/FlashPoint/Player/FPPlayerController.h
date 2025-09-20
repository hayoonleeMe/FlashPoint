// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "GameplayTagContainer.h"
#include "FPPlayerController.generated.h"

class UFPAbilitySystemComponent;
struct FInputActionValue;

/**
 * 게임플레이에서 사용할 PlayerController
 */
UCLASS()
class FLASHPOINT_API AFPPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

public:
	virtual void SetUIInputMode() override;
	virtual void SetGameplayInputMode() override;
	
protected:
	virtual void SetInitialInputMode() override;
	virtual void SetupInputComponent() override;

private:
	void Input_Move(const FInputActionValue& InputValue);
	void Input_Look(const FInputActionValue& InputValue);
	void Input_Crouch();
	void Input_PauseMenu();

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	UFPAbilitySystemComponent* GetFPAbilitySystemComponent() const;
};
