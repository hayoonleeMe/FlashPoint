// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"

#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "Player/FPPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCharacter)

AFPCharacter::AFPCharacter()
{
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void AFPCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystem();
}

void AFPCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilitySystem();
}

UAbilitySystemComponent* AFPCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAbilitySystemComponent* AFPCharacter::GetFPAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAttributeSet* AFPCharacter::GetFPAttributeSet() const
{
	return AttributeSet;
}

void AFPCharacter::InitAbilitySystem()
{
	if (AFPPlayerState* PS = GetPlayerState<AFPPlayerState>())
	{
		AbilitySystemComponent = PS->GetFPAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		AttributeSet = PS->GetFPAttributeSet();
	}
}
