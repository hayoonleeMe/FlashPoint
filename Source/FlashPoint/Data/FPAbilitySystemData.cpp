// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAbilitySystemData.h"

#include "AbilitySystemGlobals.h"
#include "FPGameplayTags.h"
#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/FPGameplayAbility.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAbilitySystemData)

void UFPAbilitySystemData::GiveDataToAbilitySystem(UFPAbilitySystemComponent* ASC) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	// Give Ability and Save Ability Handle
	for (const FFPAbilitySystemData_Ability& AbilityData : AbilitiesToGrant)
	{
		if (ensureAlways(AbilityData.AbilityClass && AbilityData.InputTag.IsValid()))
		{
			FGameplayAbilitySpec AbilitySpec(AbilityData.AbilityClass, AbilityData.AbilityLevel);
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityData.InputTag);
			const FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(AbilitySpec);
			
			ASC->GetGrantedHandles().AddAbilitySpecHandle(SpecHandle);
		}
	}

	// Apply Effect and Save Effect Handle
	for (const FFPAbilitySystemData_Effect& EffectData : EffectsToGrant)
	{
		if (EffectData.EffectClass)
		{
			const UGameplayEffect* Effect = EffectData.EffectClass->GetDefaultObject<UGameplayEffect>();
			FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectToSelf(Effect, EffectData.EffectLevel, ASC->MakeEffectContext());
			
			ASC->GetGrantedHandles().AddEffectHandle(EffectHandle);
		}
	}
}

void UFPAbilitySystemData::GiveDataToAbilitySystem(const AActor* Actor, const FName& DataId)
{
	if (const UFPAbilitySystemData* AbilitySystemData = UFPAssetManager::GetAssetById<UFPAbilitySystemData>(DataId))
	{
		if (UFPAbilitySystemComponent* ASC = Cast<UFPAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor)))
		{
			AbilitySystemData->GiveDataToAbilitySystem(ASC);
		}
	}
}

void UFPAbilitySystemData::GiveDataToAbilitySystem(const AActor* Actor, const FGameplayTag& DataTag)
{
	if (const UFPAbilitySystemData* AbilitySystemData = UFPAssetManager::GetAssetByTag<UFPAbilitySystemData>(DataTag))
	{
		if (UFPAbilitySystemComponent* ASC = Cast<UFPAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor)))
		{
			AbilitySystemData->GiveDataToAbilitySystem(ASC);
		}
	}
}

void UFPAbilitySystemData::RemoveDataFromAbilitySystem(UFPAbilitySystemComponent* ASC) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	{
		// Clear Ability and Remove Handle
		FGameplayTagContainer Tags;
		for (const FFPAbilitySystemData_Ability& AbilityData : AbilitiesToGrant)
		{
			Tags.AddTag(AbilityData.InputTag);
		}

		// 입력을 사용하지 않는 어빌리티 제거
		Tags.AddTag(FPGameplayTags::Input::NoInput);

		TArray<FGameplayAbilitySpecHandle> SpecHandles;
		ASC->FindAllAbilitiesWithInputTags(SpecHandles, Tags);

		for (const FGameplayAbilitySpecHandle& Handle : SpecHandles)
		{
			ASC->ClearAbility(Handle);
			
			ASC->GetGrantedHandles().RemoveAbilitySpecHandle(Handle);
		}
	}

	{
		// Remove Effect and Handle
		FGameplayTagContainer Tags;
		for (const FFPAbilitySystemData_Effect& EffectData : EffectsToGrant)
		{
			UGameplayEffect* EffectCDO = EffectData.EffectClass->GetDefaultObject<UGameplayEffect>();
			Tags.AppendTags(EffectCDO->GetAssetTags());
		}

		TArray<FActiveGameplayEffectHandle> EffectHandles = ASC->GetActiveEffectsWithAllTags(Tags);

		for (const FActiveGameplayEffectHandle& Handle : EffectHandles)
		{
			ASC->RemoveActiveGameplayEffect(Handle);

			ASC->GetGrantedHandles().RemoveEffectHandle(Handle);
		}
	}
}

void UFPAbilitySystemData::RemoveDataFromAbilitySystem(const AActor* Actor, const FName& DataId)
{
	if (const UFPAbilitySystemData* AbilitySystemData = UFPAssetManager::GetAssetById<UFPAbilitySystemData>(DataId))
	{
		if (UFPAbilitySystemComponent* ASC = Cast<UFPAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor)))
		{
			AbilitySystemData->RemoveDataFromAbilitySystem(ASC);
		}
	}
}

void UFPAbilitySystemData::RemoveDataFromAbilitySystem(const AActor* Actor, const FGameplayTag& DataTag)
{
	if (const UFPAbilitySystemData* AbilitySystemData = UFPAssetManager::GetAssetByTag<UFPAbilitySystemData>(DataTag))
	{
		if (UFPAbilitySystemComponent* ASC = Cast<UFPAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor)))
		{
			AbilitySystemData->RemoveDataFromAbilitySystem(ASC);
		}
	}
}
