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
