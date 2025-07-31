// Fill out your copyright notice in the Description page of Project Settings.


#include "FPInputData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPInputData)

const UInputAction* UFPInputData::FindNativeInputActionForInputTag(const FGameplayTag& InputTag) const
{
	for (const FInputConfig& InputConfig : NativeInputConfigs)
	{
		if (InputConfig.InputTag.MatchesTag(InputTag))
		{
			return InputConfig.InputAction;
		}
	}
	return nullptr;
}

const UInputAction* UFPInputData::FindAbilityInputActionForInputTag(const FGameplayTag& InputTag) const
{
	for (const FInputConfig& InputConfig : AbilityInputConfigs)
	{
		if (InputConfig.InputTag.MatchesTag(InputTag))
		{
			return InputConfig.InputAction;
		}
	}
	return nullptr;
}
