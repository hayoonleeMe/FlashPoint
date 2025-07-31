// Fill out your copyright notice in the Description page of Project Settings.


#include "FPEnhancedPlayerInput.h"

#include "EnhancedActionKeyMapping.h"
#include "Data/FPInputData.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPEnhancedPlayerInput)

void UFPEnhancedPlayerInput::FlushPressedInput(const FGameplayTag& InputTag)
{
	UWorld* World = GetWorld();
	check(World);
	
	UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);

	const UInputAction* InputAction = InputData->FindAbilityInputActionForInputTag(InputTag);
	check(InputAction);
	
	const TArray<FEnhancedActionKeyMapping>& KeyMappings = GetEnhancedActionMappings();
	for (const FEnhancedActionKeyMapping& KeyMapping : KeyMappings)
	{
		if (KeyMapping.Action == InputAction)
		{
			// InputAction에 해당하는 Key 상태를 초기화한다.
			if (FKeyState* KeyState = GetKeyStateMap().Find(KeyMapping.Key))
			{
				KeyState->RawValue = FVector(0.f, 0.f, 0.f);
				KeyState->LastUpDownTransitionTime = World->GetRealTimeSeconds();
				KeyState->bDown = false;
				KeyState->bDownPrevious = false;
			}
			return;
		}
	}
}
