// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"

#include "FPGameplayTags.h"
#include "Component/UIManageComponent.h"
#include "Components/Button.h"
#include "System/FPAssetManager.h"

void UPauseMenu::Input_UI_Back()
{
	// Resume 수행
	Button_Resume->OnClicked.Broadcast();
}

void UPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Resume->OnClicked.AddDynamic(this, &ThisClass::OnResumeButtonClicked);
	Button_Settings->OnClicked.AddDynamic(this, &ThisClass::OnSettingsButtonClicked);
	Button_Quit->OnClicked.AddDynamic(this, &ThisClass::ReturnToMainMenu);
}

void UPauseMenu::OnResumeButtonClicked()
{
	if (UUIManageComponent* UIManageComponent = UUIManageComponent::Get(GetOwningPlayer()))
	{
		UIManageComponent->RemoveWidget(EWidgetLayer::Menu, this);
	}
}

void UPauseMenu::OnSettingsButtonClicked()
{
	// TODO
}

void UPauseMenu::ReturnToMainMenu()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		const FString MainMenuLevelPath = UFPAssetManager::GetAssetPathByTag(FPGameplayTags::Asset::Level::MainMenu).GetAssetName();
		PC->ClientTravel(MainMenuLevelPath, TRAVEL_Absolute);
	}
}
