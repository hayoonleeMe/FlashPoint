// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Component/UIManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BasePlayerController)

ABasePlayerController::ABasePlayerController()
{
	bReplicates = true;
	bShowMouseCursor = true;

	UIManageComponent = CreateDefaultSubobject<UUIManageComponent>(TEXT("UI Manage Component"));
}

void ABasePlayerController::SetInitialInputMode()
{
	if (UUserWidget* MainHUDWidget = UIManageComponent->GetMainHUDWidget())
	{
		// for Menu
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(MainHUDWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInitialInputMode();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	OnInputComponentSetupDelegate.Broadcast(InputComponent);
}
