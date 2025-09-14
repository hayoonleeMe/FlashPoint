// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManageComponent.h"

#include "EnhancedInputSubsystems.h"
#include "FPGameplayTags.h"
#include "WidgetInputInteraction.h"
#include "Blueprint/UserWidget.h"
#include "Data/FPInputData.h"
#include "Input/FPInputComponent.h"
#include "Player/BasePlayerController.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIManageComponent)

UUIManageComponent::UUIManageComponent()
{
	bWantsInitializeComponent = true;
}

void UUIManageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (ABasePlayerController* BasePC = GetOwner<ABasePlayerController>())
	{
		BasePC->OnInputComponentSetupDelegate.AddUObject(this, &ThisClass::OnSetupInputComponent);
	}
}

void UUIManageComponent::BeginPlay()
{
	Super::BeginPlay();

	// UI
	if (MainWidgetClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
		{
			if (PC->IsLocalController())
			{
				MainWidget = CreateWidget(PC, MainWidgetClass);
				MainWidget->AddToViewport();
			}
		}
	}

	// UI Input
	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);

	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(InputData->UIMappingContext, 0);
			}
		}
	}
}

void UUIManageComponent::OnSetupInputComponent(UInputComponent* InputComponent)
{
	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);

	UFPInputComponent* FPInputComponent = CastChecked<UFPInputComponent>(InputComponent);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::UI::Back, ETriggerEvent::Triggered, this, &ThisClass::Input_UI_Back);
}

void UUIManageComponent::Input_UI_Back()
{
	if (IWidgetInputInteraction* WidgetInteraction = Cast<IWidgetInputInteraction>(MainWidget))
	{
		WidgetInteraction->Input_UI_Back();
	}
}
