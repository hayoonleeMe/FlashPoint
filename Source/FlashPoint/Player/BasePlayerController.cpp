// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "FPGameplayTags.h"
#include "Blueprint/UserWidget.h"
#include "Component/UIManageComponent.h"
#include "Data/FPInputData.h"
#include "Input/FPInputComponent.h"
#include "System/FPAssetManager.h"
#include "UI/WidgetInputInteraction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BasePlayerController)

ABasePlayerController::ABasePlayerController()
{
	bReplicates = true;

	UIManageComponent = CreateDefaultSubobject<UUIManageComponent>(TEXT("UI Manage Component"));
}

void ABasePlayerController::SetUIInputMode()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	SetShowMouseCursor(true);

	// UI Input
	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputData->UIMappingContext, 0);
	}
}

void ABasePlayerController::SetGameplayInputMode()
{
	SetInputMode(FInputModeGameOnly());

	SetShowMouseCursor(false);
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInitialInputMode();
}

void ABasePlayerController::SetInitialInputMode()
{
	// 기본은 UI
	SetUIInputMode();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);

	UFPInputComponent* FPInputComponent = CastChecked<UFPInputComponent>(InputComponent);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::UI::Back, ETriggerEvent::Triggered, this, &ThisClass::Input_UI_Back);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::UI::Confirm, ETriggerEvent::Triggered, this, &ThisClass::Input_UI_Confirm);
}

void ABasePlayerController::Input_UI_Back()
{
	if (IWidgetInputInteraction* WidgetInteraction = Cast<IWidgetInputInteraction>(UIManageComponent->GetTopWidget()))
	{
		WidgetInteraction->Input_UI_Back();
	}
}

void ABasePlayerController::Input_UI_Confirm()
{
	if (IWidgetInputInteraction* WidgetInteraction = Cast<IWidgetInputInteraction>(UIManageComponent->GetTopWidget()))
	{
		WidgetInteraction->Input_UI_Confirm();
	}
}
