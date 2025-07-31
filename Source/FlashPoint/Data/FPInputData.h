// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FPInputData.generated.h"

class UInputMappingContext;
class UInputAction;

USTRUCT()
struct FInputConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

/**
 * 
 */
UCLASS()
class FLASHPOINT_API UFPInputData : public UDataAsset
{
	GENERATED_BODY()

public:
	// NativeInputConfigs에서 InputTag에 해당하는 InputAction을 찾아 반환한다.
	// 찾지 못하면 nullptr를 반환한다.
	const UInputAction* FindNativeInputActionForInputTag(const FGameplayTag& InputTag) const;

	// AbilityInputConfigs에서 InputTag에 해당하는 InputAction을 찾아 반환한다.
	// 찾지 못하면 nullptr를 반환한다.
	const UInputAction* FindAbilityInputActionForInputTag(const FGameplayTag& InputTag) const;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// 일반적인 입력
	UPROPERTY(EditDefaultsOnly)
	TArray<FInputConfig> NativeInputConfigs;

	// 어빌리티를 실행하는 입력
	UPROPERTY(EditDefaultsOnly)
	TArray<FInputConfig> AbilityInputConfigs;
};
