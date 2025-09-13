// Fill out your copyright notice in the Description page of Project Settings.


#include "UserProfileWidget.h"

#include "FPGameplayTags.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "System/FPAssetManager.h"
#include "System/OnlineServiceSubsystem.h"
#include "System/PlayerAuthSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UserProfileWidget)

void UUserProfileWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
	{
		Text_Username->SetText(FText::FromString(PlayerAuthSubsystem->GetUsername()));
		Text_Email->SetText(FText::FromString(PlayerAuthSubsystem->GetEmail()));
	}

	Button_SignOut->OnClicked.AddDynamic(this, &ThisClass::OnSignOutButtonClicked);

	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (!OnlineServiceSubsystem->OnSignOutSucceededDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->OnSignOutSucceededDelegate.AddUObject(this, &ThisClass::OnSignOutSucceeded);
		}
	}
}

void UUserProfileWidget::OnSignOutButtonClicked()
{
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (UPlayerAuthSubsystem* PlayerAuthSubsystem = UPlayerAuthSubsystem::Get(this))
		{
			OnlineServiceSubsystem->SignOut(PlayerAuthSubsystem->GetAuthResult().AccessToken);
			
			Button_SignOut->SetIsEnabled(false);
		}
	}
}

void UUserProfileWidget::OnSignOutSucceeded()
{
	const FName SignScreenLevelPath = UFPAssetManager::GetAssetPathByTag(FPGameplayTags::Asset::Level::SignScreen).GetAssetFName();
	UGameplayStatics::OpenLevel(this, SignScreenLevelPath);
}
