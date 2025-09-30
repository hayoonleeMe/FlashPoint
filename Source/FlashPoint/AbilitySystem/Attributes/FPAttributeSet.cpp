// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Game/BaseGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/FPPlayerState.h"

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

	// Target Dead
	if (NewHealth <= 0.f && GetWorld())
	{
		if (ABaseGameState* BaseGS = GetWorld()->GetGameState<ABaseGameState>())
		{
			if (AFPPlayerState* TargetPS = Cast<AFPPlayerState>(GetOwningActor()))
			{
				// 죽은 플레이어의 Death Count +1
				TargetPS->AddToDeathCount(1);

				BaseGS->UpdatePlayerInfo(TargetPS->MakePlayerInfo());
			}
			if (AFPPlayerState* InstigatorPS = Cast<AFPPlayerState>(Data.EffectSpec.GetEffectContext().GetInstigator()))
			{
				// 죽인 플레이어의 Kill Count +1
				InstigatorPS->AddToKillCount(1);

				BaseGS->UpdatePlayerInfo(InstigatorPS->MakePlayerInfo());
			}
		}
	}
	// TODO : 피격 효과, HUD, Respawn
}

void UFPAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPAttributeSet, Health, OldHealth);
}

void UFPAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFPAttributeSet, MaxHealth, OldMaxHealth);
}
