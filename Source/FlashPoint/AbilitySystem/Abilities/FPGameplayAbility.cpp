// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameplayAbility.h"

#include "Input/FPEnhancedPlayerInput.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility)

UFPGameplayAbility::UFPGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

FGameplayTag UFPGameplayAbility::GetInputTag() const
{
	if (FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec())
	{
		// Ability 마다 하나의 Input Tag를 가짐
		FGameplayTag SubInputTag(FGameplayTag::RequestGameplayTag(TEXT("Input")));
		FGameplayTagContainer InputTag = AbilitySpec->DynamicAbilityTags.Filter(SubInputTag.GetSingleTagContainer());
		return InputTag.First();
	}
	return FGameplayTag();
}

void UFPGameplayAbility::FlushPressedInput() const
{
	FlushPressedInput(GetInputTag());
}

void UFPGameplayAbility::FlushPressedInput(const FGameplayTag& InputTag) const
{
	if (APlayerController* PlayerController = GetPlayerController())
	{
		if (UFPEnhancedPlayerInput* PlayerInput = Cast<UFPEnhancedPlayerInput>(PlayerController->PlayerInput))
		{
			PlayerInput->FlushPressedInput(InputTag);
		}
	}
}

APlayerController* UFPGameplayAbility::GetPlayerController() const
{
	return CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr;
}
