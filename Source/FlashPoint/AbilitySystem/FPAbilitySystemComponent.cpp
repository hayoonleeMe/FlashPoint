// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAbilitySystemComponent.h"

#include "Abilities/FPGameplayAbility.h"
#include "Animation/FPAnimInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAbilitySystemComponent)

void UFPAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	bool bHasNewAvatarPawn = InAvatarActor->IsA<APawn>() && InAvatarActor != ActorInfo->AvatarActor;
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	// 이 함수는 컴포넌트 초기화, 외부 호출, Rep Notify 등 여러번 호출되므로 AvatarActor가 실제로 유효한 Pawn으로 등록될 때만 아래 초기화 로직을 수행한다.
	if (bHasNewAvatarPawn)
	{
		if (UFPAnimInstance* FPAnimInstance = Cast<UFPAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			FPAnimInstance->InitializeWithAbilitySystem(this);
		}
	}
}

void UFPAbilitySystemComponent::FindAllAbilitiesWithInputTags(TArray<FGameplayAbilitySpecHandle>& OutAbilityHandles, const FGameplayTagContainer& Tags) const
{
	OutAbilityHandles.Empty();

	for (const FGameplayAbilitySpec& CurrentSpec : ActivatableAbilities.Items)
	{
		if (CurrentSpec.DynamicAbilityTags.HasAnyExact(Tags))
		{
			OutAbilityHandles.Add(CurrentSpec.Handle);
		}
	}
}

void UFPAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFPAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		// @see UAbilityTask_WaitInputPress
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UFPAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		// @see UAbilityTask_WaitInputPress
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

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

void UFPAbilitySystemComponent::ProcessAbilityInput()
{
	// TODO : Block Process Input?

	// 배열의 capacity를 유지해 메모리 할당을 줄임
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// 입력이 유지될 때 활성화되는 어빌리티 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (!Spec->IsActive())
			{
				const UFPGameplayAbility* AbilityCDO = CastChecked<UFPGameplayAbility>(Spec->Ability);
				if (AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
				}
			}
		}
	}

	// 이번 프레임에 input press된 어빌리티 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			Spec->InputPressed = true;
			
			if (Spec->IsActive())
			{
				AbilitySpecInputPressed(*Spec);
			}
			else
			{
				const UFPGameplayAbility* AbilityCDO = CastChecked<UFPGameplayAbility>(Spec->Ability);
				if (AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
				}
			}
		}
	}

	// 어빌리티 활성화
	for (const FGameplayAbilitySpecHandle& SpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(SpecHandle);
	}

	// 이번 프레임에 input release된 어빌리티 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			Spec->InputPressed = false;
			
			if (Spec->IsActive())
			{
				AbilitySpecInputReleased(*Spec);
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}
