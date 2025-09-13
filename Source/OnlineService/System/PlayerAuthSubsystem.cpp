// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAuthSubsystem.h"

#include "OnlineServiceInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerAuthSubsystem)

UPlayerAuthSubsystem* UPlayerAuthSubsystem::Get(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (ULocalPlayer* LocalPlayer = GameInstance->GetFirstGamePlayer())
			{
				return LocalPlayer->GetSubsystem<UPlayerAuthSubsystem>();
			}
		}
	}
	return nullptr;
}

void UPlayerAuthSubsystem::InitializeTokens(const FString& InUsername, const FString& InEmail, const FAuthenticationResult& AuthResult, const TScriptInterface<IOnlineServiceInterface>& InOnlineServiceInterface)
{
	Username = InUsername;
	Email = InEmail;
	AuthenticationResult = AuthResult;
	OnlineServiceInterface = InOnlineServiceInterface;
	SetRefreshTokenTimer();
}

void UPlayerAuthSubsystem::UpdateTokens(const FString& AccessToken, const FString& IdToken)
{
	AuthenticationResult.AccessToken = AccessToken;
	AuthenticationResult.IdToken = IdToken;
	AuthenticationResult.Dump();
	SetRefreshTokenTimer();
}

void UPlayerAuthSubsystem::SetRefreshTokenTimer()
{
	GetWorld()->GetTimerManager().SetTimer(RefreshTimer, FTimerDelegate::CreateUObject(this, &ThisClass::RefreshTokens), TokenRefreshInterval, false);
}

void UPlayerAuthSubsystem::RefreshTokens()
{
	if (OnlineServiceInterface)
	{
		OnlineServiceInterface->RefreshTokens(AuthenticationResult.RefreshToken);
	}
}
