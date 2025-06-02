// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerState.h"

#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FPAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPPlayerState)

AFPPlayerState::AFPPlayerState()
{
	NetUpdateFrequency = 120.f;
	MinNetUpdateFrequency = 60.f;
	
	AbilitySystemComponent = CreateDefaultSubobject<UFPAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UFPAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AFPPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAbilitySystemComponent* AFPPlayerState::GetFPAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAttributeSet* AFPPlayerState::GetFPAttributeSet() const
{
	return AttributeSet;
}
