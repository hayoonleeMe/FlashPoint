// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FPAbilitySystemData.generated.h"

class UFPAbilitySystemComponent;
class UGameplayEffect;
class UFPGameplayAbility;

USTRUCT()
struct FFPAbilitySystemData_Ability
{
	GENERATED_BODY()

	FFPAbilitySystemData_Ability()
		: AbilityLevel(1)
	{ }

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

USTRUCT()
struct FFPAbilitySystemData_Effect
{
	GENERATED_BODY()
	
	FFPAbilitySystemData_Effect()
		: EffectLevel(1)
	{ }

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectClass;
	
	UPROPERTY(EditDefaultsOnly)
	int32 EffectLevel;
};

/**
 * 부여된 Ability Handle, 적용된 Effect Handle을 저장한다.
 */
USTRUCT()
struct FFPAbilitySystemData_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
	{
		if (Handle.IsValid())
		{
			GrantedAbilitySpecHandles.Add(Handle);
		}
	}
	
	void AddEffectHandle(const FActiveGameplayEffectHandle& Handle)
	{
		if (Handle.IsValid())
		{
			GrantedEffectHandles.Add(Handle);
		}
	}

	void RemoveAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
	{
		if (Handle.IsValid())
		{
			GrantedAbilitySpecHandles.Remove(Handle);
		}
	}

	void RemoveEffectHandle(const FActiveGameplayEffectHandle& Handle)
	{
		if (Handle.IsValid())
		{
			GrantedEffectHandles.Remove(Handle);
		}
	}
		
protected:
	// 부여된 Ability Spec Handle
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;

	// 적용된 Effect Handle
	TArray<FActiveGameplayEffectHandle> GrantedEffectHandles;
};

/**
 * 부여할 Ability와 적용할 Effect를 저장하는 DataAsset
 */
UCLASS(Const)
class FLASHPOINT_API UFPAbilitySystemData : public UDataAsset
{
	GENERATED_BODY()

public:
	// ASC에 AbilitiesToGrant 어빌리티를 부여하고 EffectsToGrant 이펙트를 적용한다.
	// Server Only
	void GiveDataToAbilitySystem(UFPAbilitySystemComponent* ASC) const;

	// Actor에 DataId로 등록된 Ability System Data를 설정한다.
	// Server Only
	static void GiveDataToAbilitySystem(const AActor* Actor, const FName& DataId);

	// Actor에 DataTag로 등록된 Ability System Data를 설정한다.
	// Server Only
	static void GiveDataToAbilitySystem(const AActor* Actor, const FGameplayTag& DataTag);

	// ASC에서 부여된 AbilitiesToGrant 어빌리티와 EffectsToGrant 이펙트를 제거한다.
	// Server Only
	void RemoveDataFromAbilitySystem(UFPAbilitySystemComponent* ASC) const;

	// Actor에 DataId로 등록된 Ability System Data를 제거한다.
	// Server Only
	static void RemoveDataFromAbilitySystem(const AActor* Actor, const FName& DataId);

	// Actor에 DataTag로 등록된 Ability System Data를 제거한다.
	// Server Only
	static void RemoveDataFromAbilitySystem(const AActor* Actor, const FGameplayTag& DataTag);

	// 부여할 어빌리티
	UPROPERTY(EditDefaultsOnly)
	TArray<FFPAbilitySystemData_Ability> AbilitiesToGrant;

	// 적용할 이펙트
	UPROPERTY(EditDefaultsOnly)
	TArray<FFPAbilitySystemData_Effect> EffectsToGrant;
};
