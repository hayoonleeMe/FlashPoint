// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManageComponent.h"

#include "EnhancedInputSubsystems.h"
#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "Blueprint/UserWidget.h"
#include "Data/FPInputData.h"
#include "Input/FPInputComponent.h"
#include "Player/BasePlayerController.h"
#include "System/FPAssetManager.h"
#include "UI/WidgetInputInteraction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIManageComponent)

UUIManageComponent* UUIManageComponent::Get(const APlayerController* OwningPC)
{
	return OwningPC ? OwningPC->FindComponentByClass<UUIManageComponent>() : nullptr;
}

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

UUserWidget* UUIManageComponent::GetMainHUDWidget() const
{
	if (const FActiveWidgetArray* Array = ActiveWidgetMap.Find(EWidgetLayer::HUD))
	{
		for (UUserWidget* Widget : Array->Widgets)
		{
			if (Widget->IsA(MainHUDWidgetClass))
			{
				return Widget;
			}
		}
	}
	return nullptr;
}

UUserWidget* UUIManageComponent::AddWidget(EWidgetLayer WidgetLayer, const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (WidgetLayer != EWidgetLayer::MAX && WidgetClass)
	{
		if (APlayerController* PC = GetOwner<APlayerController>())
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass);
			Widget->AddToViewport(static_cast<int32>(WidgetLayer));

			FActiveWidgetArray& ActiveWidgetStack = ActiveWidgetMap.FindOrAdd(WidgetLayer);
			ActiveWidgetStack.Widgets.Add(Widget);
					
			return Widget;
		}
	}
			
	UE_LOG(LogFP, Error, TEXT("[%hs] Invalid WidgetLayer or WidgetClass."), __FUNCTION__);
	return nullptr;
}

void UUIManageComponent::RemoveWidgets(EWidgetLayer WidgetLayer, const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (WidgetLayer != EWidgetLayer::MAX && WidgetClass)
	{
		if (FActiveWidgetArray* ActiveWidgetArray = ActiveWidgetMap.Find(WidgetLayer))
		{
			auto Predicate = [&WidgetClass](const UUserWidget* Target) -> bool
			{
				return Target && Target->IsA(WidgetClass);
			};
			ActiveWidgetArray->Widgets.RemoveAll(Predicate);
		}
	}
	else
	{
		UE_LOG(LogFP, Error, TEXT("[%hs] Invalid WidgetLayer or WidgetClass."), __FUNCTION__);
	}
}

void UUIManageComponent::RemoveWidget(EWidgetLayer WidgetLayer, UUserWidget* WidgetToRemove)
{
	if (WidgetLayer != EWidgetLayer::MAX && WidgetToRemove)
	{
		if (FActiveWidgetArray* ActiveWidgetArray = ActiveWidgetMap.Find(WidgetLayer))
		{
			if (!ActiveWidgetArray->Widgets.IsEmpty())
			{
				if (WidgetToRemove == ActiveWidgetArray->Widgets.Last())
				{
					ActiveWidgetArray->Widgets.Pop();
				}
				else
				{
					ActiveWidgetArray->Widgets.Remove(WidgetToRemove);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogFP, Error, TEXT("[%hs] Invalid WidgetLayer or WidgetToRemove."), __FUNCTION__);
	}
}

void UUIManageComponent::BeginPlay()
{
	Super::BeginPlay();

	// UI
	AddWidget(EWidgetLayer::HUD, MainHUDWidgetClass);

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
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::UI::Confirm, ETriggerEvent::Triggered, this, &ThisClass::Input_UI_Confirm);
}

UUserWidget* UUIManageComponent::GetTopWidget() const
{
	for (int32 Index = static_cast<int32>(EWidgetLayer::MAX) - 1; Index >= 0; --Index)
	{
		EWidgetLayer WidgetLayer = static_cast<EWidgetLayer>(Index);
		if (const FActiveWidgetArray* ActiveWidgetArray = ActiveWidgetMap.Find(WidgetLayer))
		{
			if (!ActiveWidgetArray->Widgets.IsEmpty())
			{
				return ActiveWidgetArray->Widgets.Last();
			}
		}
	}
	return nullptr;
}

void UUIManageComponent::Input_UI_Back()
{
	if (IWidgetInputInteraction* WidgetInteraction = Cast<IWidgetInputInteraction>(GetTopWidget()))
	{
		WidgetInteraction->Input_UI_Back();
	}
}

void UUIManageComponent::Input_UI_Confirm()
{
	if (IWidgetInputInteraction* WidgetInteraction = Cast<IWidgetInputInteraction>(GetTopWidget()))
	{
		WidgetInteraction->Input_UI_Confirm();
	}
}
