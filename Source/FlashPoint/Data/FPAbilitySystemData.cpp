// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAbilitySystemData.h"

#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/FPGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAbilitySystemData)

void UFPAbilitySystemData::GiveDataToAbilitySystem(UFPAbilitySystemComponent* ASC, FFPAbilitySystemData_GrantedHandles* OutGrantedHandles) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	// Give Ability and Save Ability Handle
	for (const FFPAbilitySystemData_Ability& AbilityData : AbilitiesToGrant)
	{
		if (AbilityData.AbilityClass && AbilityData.InputTag.IsValid())
		{
			FGameplayAbilitySpec AbilitySpec(AbilityData.AbilityClass, AbilityData.AbilityLevel);
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityData.InputTag);
			const FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(AbilitySpec);
			
			if (OutGrantedHandles)
			{
				OutGrantedHandles->AddAbilitySpecHandle(SpecHandle);
			}
		}
	}

	// Apply Effect and Save Effect Handle
	for (const FFPAbilitySystemData_Effect& EffectData : EffectsToGrant)
	{
		if (EffectData.EffectClass)
		{
			const UGameplayEffect* Effect = EffectData.EffectClass->GetDefaultObject<UGameplayEffect>();
			FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectToSelf(Effect, EffectData.EffectLevel, ASC->MakeEffectContext());
			
			if (OutGrantedHandles)
			{
				OutGrantedHandles->AddEffectHandle(EffectHandle);
			}			
		}
	}
}
