// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAbilitySystemComponent)

void UFPAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

{

void UFPAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				InputPressedSpecHandles.AddUnique(Spec.Handle);
				InputHeldSpecHandles.AddUnique(Spec.Handle);
			}
		}
	}
}

void UFPAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				// 해당 어빌리티
				InputReleasedSpecHandles.AddUnique(Spec.Handle);
				InputHeldSpecHandles.Remove(Spec.Handle);
			}
		}
	}
}
}
