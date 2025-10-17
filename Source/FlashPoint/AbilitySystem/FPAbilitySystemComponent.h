// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FPAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UFPAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	// Tags에 해당하는 Input Tag를 가지는 모든 Ability Spec의 Handle을 반환한다. 
	void FindAllAbilitiesWithInputTags(TArray<FGameplayAbilitySpecHandle>& OutAbilityHandles, const FGameplayTagContainer& Tags) const;

	// InputTag에 해당하는 어빌리티에 연동된 입력을 Flush한다.
	// 입력이 존재하는 로컬 플레이어에 대해서만 수행한다.
	void FlushPressedInput(const FGameplayTag& InputTag);

	// AvatarActor가 Locally Controlled 인지 반환한다.
	bool IsAvatarLocallyControlled() const;

protected:
	virtual void BeginPlay() override;

public:
	// AbilityFailedCallbacks 델레게이트에 등록할 Callback
	// Ability 실행이 실패할 때 호출된다.
	void OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureTags);
	
	/// 어빌리티를 Input Pressed로 설정하고, InputPressed Replicated Event를 호출한다.
	/// AbilityReplicatedEventDelegate() 델레게이트로 Event를 받을 수 있다.
	/// @see UAbilityTask_WaitInputPress
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;

	/// 어빌리티를 Input Released로 설정하고, InputReleased Replicated Event를 호출한다.
	/// AbilityReplicatedEventDelegate() 델레게이트로 Event를 받을 수 있다.
	/// @see UAbilityTask_WaitInputRelease
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	
	/// InputTag에 해당하는 입력이 Press되면 호출된다.
	/// 이번 프레임에서 Input Press된 Ability Spec Handle을 저장하고, ProcessAbilityInput()에서 처리한다.
	/// @see AFPPlayerController::Input_AbilityInputTagPressed
	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	/// InputTag에 해당하는 입력이 Release되면 호출된다.
	/// 이번 프레임에서 Input Released된 Ability Spec Handle을 저장하고, ProcessAbilityInput()에서 처리한다.
	/// @see AFPPlayerController::Input_AbilityInputTagReleased
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
};
