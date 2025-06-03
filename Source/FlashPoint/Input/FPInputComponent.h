// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Data/FPInputData.h"
#include "FPInputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UFPInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// InputTag에 해당하는 Native InputAction의 TriggerEvent에 Func를 등록한다.
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UFPInputData* InputData, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

	// InputData에 저장된 모든 Ability InputAction에 Triggered Event에 PressedFunc, Completed Event에 ReleasedFunc를 등록한다.
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UFPInputData* InputData, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
};

template <class UserClass, typename FuncType>
void UFPInputComponent::BindNativeAction(const UFPInputData* InputData, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	check(InputData);

	if (const UInputAction* InputAction = InputData->FindNativeInputActionForInputTag(InputTag))
	{
		BindAction(InputAction, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UFPInputComponent::BindAbilityActions(const UFPInputData* InputData, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputData);

	for (const FInputConfig& InputConfig : InputData->AbilityInputConfigs)
	{
		if (InputConfig.InputAction && InputConfig.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(InputConfig.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, InputConfig.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(InputConfig.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputConfig.InputTag);
			}
		}
	}
}
