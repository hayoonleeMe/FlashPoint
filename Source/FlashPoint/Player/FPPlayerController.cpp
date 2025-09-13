// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "FPGameplayTags.h"
#include "FPPlayerState.h"
#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "Data/FPInputData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/FPInputComponent.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPPlayerController)

AFPPlayerController::AFPPlayerController()
{
	bReplicates = true;
}

void AFPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputData->DefaultMappingContext, 0);
	}
}

void AFPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);

	UFPInputComponent* FPInputComponent = CastChecked<UFPInputComponent>(InputComponent);

	// Bind Native Inputs
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Action::Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Action::Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Action::Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch);

	// Bind Ability Inputs
	FPInputComponent->BindAbilityActions(InputData, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);
}

void AFPPlayerController::Input_Move(const FInputActionValue& InputValue)
{
	if (GetPawn())
	{
		const FVector2D MovementVector = InputValue.Get<FVector2D>();
		const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

		if (MovementVector.X != 0.f)
		{
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			GetPawn()->AddMovementInput(ForwardDirection, MovementVector.X);
		}
		if (MovementVector.Y != 0.f)
		{
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			GetPawn()->AddMovementInput(RightDirection, -MovementVector.Y);	
		}
	}
}

void AFPPlayerController::Input_Look(const FInputActionValue& InputValue)
{
	const FVector2D LookAxisVector = InputValue.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(-LookAxisVector.Y);
}

void AFPPlayerController::Input_Crouch()
{
	if (GetCharacter() && GetCharacter()->GetCharacterMovement()->IsMovingOnGround())
	{
		GetCharacter()->bIsCrouched ? GetCharacter()->UnCrouch() : GetCharacter()->Crouch();  
	}
}

void AFPPlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UFPAbilitySystemComponent* ASC = GetFPAbilitySystemComponent())
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AFPPlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UFPAbilitySystemComponent* ASC = GetFPAbilitySystemComponent())
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

UFPAbilitySystemComponent* AFPPlayerController::GetFPAbilitySystemComponent() const
{
	if (const AFPPlayerState* PS = GetPlayerState<AFPPlayerState>())
	{
		return PS->GetFPAbilitySystemComponent();
	}
	return nullptr;
}
