// Fill out your copyright notice in the Description page of Project Settings.


#include "FPInputData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPInputData)

const UInputAction* UFPInputData::FindInputActionForInputTag(const FGameplayTag& InputTag) const
{
	return InputTagToInputActionMap.FindRef(InputTag); 
}

void UFPInputData::PostLoad()
{
	Super::PostLoad();
	
	RebuildCache();
}

#if WITH_EDITOR
void UFPInputData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	RebuildCache();
}
#endif

void UFPInputData::RebuildCache()
{
	InputTagToInputActionMap.Empty(NativeInputConfigs.Num() + AbilityInputConfigs.Num());

	for (const FInputConfig& InputConfig : NativeInputConfigs)
	{
		if (InputConfig.InputTag.IsValid() && InputConfig.InputAction)
		{
			InputTagToInputActionMap.Add(InputConfig.InputTag, InputConfig.InputAction);
		}
	}
	
	for (const FInputConfig& InputConfig : AbilityInputConfigs)
	{
		if (InputConfig.InputTag.IsValid() && InputConfig.InputAction)
		{
			InputTagToInputActionMap.Add(InputConfig.InputTag, InputConfig.InputAction);
		}
	}
}
