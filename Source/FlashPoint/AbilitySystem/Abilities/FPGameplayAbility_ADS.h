// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "FPGameplayAbility.h"
// #include "FPGameplayAbility_ADS.generated.h"
//
// class AWeapon_Base;
//
// /**
//  * ADS (정조준)을 수행하는 어빌리티
//  */
// UCLASS()
// class FLASHPOINT_API UFPGameplayAbility_ADS : public UFPGameplayAbility
// {
// 	GENERATED_BODY()
//
// public:
// 	UFPGameplayAbility_ADS();
//
// 	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
//
// protected:
// 	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
// 	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
// 	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
// 	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
//
// 	static AWeapon_Base* GetEquippedWeapon(const AActor* AvatarActor);
//
// private:
// 	UPROPERTY(EditDefaultsOnly)
// 	float CameraBlendTime;
//
// 	UPROPERTY(EditDefaultsOnly)
// 	float CameraFOV;
// };
