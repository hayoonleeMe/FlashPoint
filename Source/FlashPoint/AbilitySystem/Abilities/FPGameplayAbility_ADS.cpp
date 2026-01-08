// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "FPGameplayAbility_ADS.h"
//
// #include "FPGameplayTags.h"
// #include "FPLogChannels.h"
// #include "Character/FPCharacter.h"
// #include "Weapon/WeaponManageComponent.h"
// #include "Weapon/Weapon_Base.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameplayAbility_ADS)
//
// UFPGameplayAbility_ADS::UFPGameplayAbility_ADS()
// {
// 	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
// 	AbilityTags.AddTag(FPGameplayTags::Ability::ADS);
// 	ActivationOwnedTags.AddTag(FPGameplayTags::CharacterState::IsADS);
// 	ActivationBlockedTags.AddTag(FPGameplayTags::Weapon::NoFire);
//
// 	CameraBlendTime = 0.1f;
// 	CameraFOV = 80.f;
// }
//
// void UFPGameplayAbility_ADS::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
// 	const FGameplayAbilityActivationInfo ActivationInfo)
// {
// 	// TODO: 로컬온리면 여기서, 애님몽타주 재생으로 인한 local predicted라면 여기 말고 UAbilityTask_WaitInputRelease로
// 	//K2_EndAbility();
// }
//
// bool UFPGameplayAbility_ADS::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
//                                                 const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
// {
// 	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
// 	{
// 		return false;
// 	}
//
// 	// 장착 중인 무기가 유효한지 체크
// 	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
// 	if (!IsValid(GetEquippedWeapon(AvatarActor)))
// 	{
// 		UE_LOG(LogFP, Warning, TEXT("[%hs] Can't activate ability because of invalid equipped weapon."), __FUNCTION__);
// 		return false;
// 	}
//
// 	return true;
// }
//
// void UFPGameplayAbility_ADS::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
//                                              const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
// {
// 	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
//
// 	// if (AFPCharacter* FPCharacter = Cast<AFPCharacter>(GetAvatarActorFromActorInfo()))
// 	// {
// 	// 	FPCharacter->ChangeView(true);
// 	// }
//
// 	/*if (APlayerController* PC = GetPlayerController())
// 	{
// 		if (AFPPlayerCameraManager* PlayerCameraManager = Cast<AFPPlayerCameraManager>(PC->PlayerCameraManager))
// 		{
// 			if (AWeapon_Base* Weapon = GetEquippedWeapon(GetAvatarActorFromActorInfo()))
// 			{
// 				// FVector WorldLocation = PlayerCameraManager->GetCameraLocation();
// 				// FTransform SocketTransform = Weapon->GetADSSocketTransform();
// 				// FVector SocketForwardVector = SocketTransform.GetRotation().GetForwardVector();
// 				// FVector CameraToSocket = SocketTransform.GetLocation() - WorldLocation;
// 				// FVector CameraToSocketNormal = CameraToSocket.GetSafeNormal();
// 				// float Dot = CameraToSocketNormal.Dot(SocketForwardVector);
// 				// float Length = CameraToSocket.Length() / 250.f;
// 				// FVector Target = WorldLocation - SocketForwardVector * Dot * Length;
// 				PlayerCameraManager->SetCameraUpdate(ECameraSetType::Blend, Weapon->GetRootComponent(), Weapon->GetADSSocketName(), CameraFOV, CameraBlendTime);
// 			}
// 		}
// 	}*/
// }
//
// void UFPGameplayAbility_ADS::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
// 	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
// {
// 	// if (AFPCharacter* FPCharacter = Cast<AFPCharacter>(GetAvatarActorFromActorInfo()))
// 	// {
// 	// 	FPCharacter->ChangeView(false);
// 	// }
// 	
// 	// if (APlayerController* PC = GetPlayerController())
// 	// {
// 	// 	if (AFPPlayerCameraManager* PlayerCameraManager = Cast<AFPPlayerCameraManager>(PC->PlayerCameraManager))
// 	// 	{
// 	// 		PlayerCameraManager->SetToDefault(ECameraSetType::Blend, CameraBlendTime);
// 	// 	}
// 	// }
// 	
// 	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
// }
//
// void UFPGameplayAbility_ADS::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
// 	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
// {
// 	// if (APlayerController* PC = GetPlayerController())
// 	// {
// 	// 	if (AFPPlayerCameraManager* PlayerCameraManager = Cast<AFPPlayerCameraManager>(PC->PlayerCameraManager))
// 	// 	{
// 	// 		PlayerCameraManager->SetToDefault(ECameraSetType::Blend, CameraBlendTime);
// 	// 	}
// 	// }
// 	
// 	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
// }
//
// AWeapon_Base* UFPGameplayAbility_ADS::GetEquippedWeapon(const AActor* AvatarActor)
// {
// 	if (AvatarActor)
// 	{
// 		if (UWeaponManageComponent* WeaponManageComp = AvatarActor->FindComponentByClass<UWeaponManageComponent>())
// 		{
// 			return WeaponManageComp->GetEquippedWeapon();
// 		}
// 	}
// 	return nullptr;
// }
