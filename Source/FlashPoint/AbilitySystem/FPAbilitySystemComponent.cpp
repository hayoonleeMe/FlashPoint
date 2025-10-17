// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAbilitySystemComponent.h"

#include "FPGameplayTags.h"
#include "Abilities/FPGameplayAbility.h"
#include "Animation/FPAnimInstance.h"
#include "Weapon/WeaponManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAbilitySystemComponent)

void UFPAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	bool bHasNewAvatarPawn = IsValid(InAvatarActor) && InAvatarActor->IsA<APawn>() && InAvatarActor != ActorInfo->AvatarActor;
	
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

void UFPAbilitySystemComponent::FlushPressedInput(const FGameplayTag& InputTag)
{
	if (!IsAvatarLocallyControlled())
	{
		return;
	}
	
	for (const FGameplayAbilitySpec& CurrentSpec : ActivatableAbilities.Items)
	{
		if (CurrentSpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			if (UFPGameplayAbility* AbilityInstance = Cast<UFPGameplayAbility>(CurrentSpec.GetPrimaryInstance()))
			{
				AbilityInstance->FlushPressedInput(InputTag);
			}
		}
	}
}

bool UFPAbilitySystemComponent::IsAvatarLocallyControlled() const
{
	if (APawn* AvatarPawn = Cast<APawn>(GetAvatarActor()))
	{
		return AvatarPawn->IsLocallyControlled();
	}
	return false;
}

void UFPAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsAvatarLocallyControlled())
	{
		AbilityFailedCallbacks.AddUObject(this, &ThisClass::OnAbilityFailed);
	}
}

void UFPAbilitySystemComponent::OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureTags)
{
	// NoAmmo 때문에 실패한 경우에만
	if (FailureTags.Num() == 1 && FailureTags.HasTagExact(FPGameplayTags::Ability::Fail::NoAmmo))
	{
		const int32 ReserveAmmo = UWeaponManageComponent::GetReserveAmmoStackCount(GetAvatarActor());
		if (ReserveAmmo <= 0)
		{
			// Ammo와 ReserveAmmo 모두 없으므로 Dry Fire를 수행한다.
			// TODO : Dry Fire
			UE_LOG(LogTemp, Warning, TEXT("%hs, DryFire"), __FUNCTION__);
		}
		else
		{
			// Ammo만 없으므로 재장전을 수행한다.
			TryActivateAbilitiesByTag(FGameplayTagContainer(FPGameplayTags::Ability::Reload), false);
		}
	}
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
		ABILITYLIST_SCOPE_LOCK();
		
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				if (!Spec.IsActive())
				{
					TryActivateAbility(Spec.Handle);
				}
				AbilitySpecInputPressed(Spec);
			}
		}
	}
}

void UFPAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		ABILITYLIST_SCOPE_LOCK();
		
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				if (Spec.IsActive())
				{
					AbilitySpecInputReleased(Spec);
				}
			}
		}
	}
}
