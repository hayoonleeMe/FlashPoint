// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAttributeSet)

UFPAttributeSet::UFPAttributeSet()
{
}

void UFPAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFPAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFPAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UFPAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UFPAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Data);
	}
}

void UFPAttributeSet::HandleIncomingDamage(const FGameplayEffectModCallbackData& Data)
{
	float LocalDamage = Data.EvaluatedData.Magnitude;
	float NewHealth = GetHealth() - LocalDamage;
	SetHealth(NewHealth);

	// TODO : 피격 효과, HUD
}

void UFPAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPAttributeSet, Health, OldHealth);
}

void UFPAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPAttributeSet, MaxHealth, OldMaxHealth);
}
