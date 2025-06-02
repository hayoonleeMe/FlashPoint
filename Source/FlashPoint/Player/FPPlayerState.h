// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "FPPlayerState.generated.h"

class UFPAttributeSet;
class UFPAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class FLASHPOINT_API AFPPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFPPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFPAbilitySystemComponent* GetFPAbilitySystemComponent() const;
	UFPAttributeSet* GetFPAttributeSet() const;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFPAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFPAttributeSet> AttributeSet;
};
