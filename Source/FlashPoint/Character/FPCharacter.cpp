// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"

#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FPPlayerState.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCharacter)

AFPCharacter::AFPCharacter()
{
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	BaseEyeHeight = 80.f;
	CrouchedEyeHeight = 50.f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->GravityScale = 1.f;
	GetCharacterMovement()->MaxAcceleration = 2400.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.f;
	GetCharacterMovement()->BrakingFriction = 6.f;
	GetCharacterMovement()->GroundFriction = 8.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1400.f;
	GetCharacterMovement()->RotationRate.Yaw = 720.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.f);
}

void AFPCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystem();
}

void AFPCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilitySystem();
}

UAbilitySystemComponent* AFPCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAbilitySystemComponent* AFPCharacter::GetFPAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAttributeSet* AFPCharacter::GetFPAttributeSet() const
{
	return AttributeSet;
}

void AFPCharacter::InitAbilitySystem()
{
	if (AFPPlayerState* PS = GetPlayerState<AFPPlayerState>())
	{
		AbilitySystemComponent = PS->GetFPAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		AttributeSet = PS->GetFPAttributeSet();
	}

	ApplyAbilitySystemData(TEXT("ASD_Default"));
}

void AFPCharacter::ApplyAbilitySystemData(const FName& DataId)
{
	if (HasAuthority())
	{
		if (const UFPAbilitySystemData* AbilitySystemData = UFPAssetManager::GetAssetById<UFPAbilitySystemData>(DataId))
		{
			AbilitySystemData->GiveDataToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
		}
	}
}
