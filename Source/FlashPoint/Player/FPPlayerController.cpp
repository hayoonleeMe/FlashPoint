// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Data/FPInputData.h"
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
}
