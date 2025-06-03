// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FPAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UFPAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	/// InputTag에 해당하는 입력이 Press되면 호출된다.
	/// 이번 프레임에서 Input Press된 Ability Spec Handle을 저장하고, ProcessAbilityInput()에서 처리한다.
	/// @see AFPPlayerController::Input_AbilityInputTagPressed
	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	/// InputTag에 해당하는 입력이 Release되면 호출된다.
	/// 이번 프레임에서 Input Released된 Ability Spec Handle을 저장하고, ProcessAbilityInput()에서 처리한다.
	/// @see AFPPlayerController::Input_AbilityInputTagReleased
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
private:
	// 이번 프레임에 Input Press된 Ability Spec Handle
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// 입력 유지 중인 Ability Spec Handle
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// 이번 프레임에 Input Release된 Ability Spec Handle
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
};
