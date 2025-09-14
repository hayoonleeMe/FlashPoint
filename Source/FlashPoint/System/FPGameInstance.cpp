// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameInstance.h"

#include "FPAssetManager.h"
#include "System/OnlineServiceSubsystem.h"
#include "System/PlayerAuthSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameInstance)

void UFPGameInstance::Init()
{
	Super::Init();

	UFPAssetManager::Initialize();
}

void UFPGameInstance::Shutdown()
{
#if !UE_SERVER
	// 게임 종료 전 로그인 중이라면 Sign Out
	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = GetSubsystem<UOnlineServiceSubsystem>())
	{
		if (ULocalPlayer* LocalPlayer = GetFirstGamePlayer())
		{
			if (UPlayerAuthSubsystem* PlayerAuthSubsystem = LocalPlayer->GetSubsystem<UPlayerAuthSubsystem>())
			{
				const FString& AccessToken = PlayerAuthSubsystem->GetAuthResult().AccessToken;
				if (!AccessToken.IsEmpty())
				{
					OnlineServiceSubsystem->SignOut(AccessToken);
				}
			}
		}
	}
#endif

	Super::Shutdown();
}

void UFPGameInstance::CacheMatchInfo(const FMatchInfo& InMatchInfo)
{
	CachedMatchInfo = InMatchInfo;
}

FMatchInfo UFPGameInstance::ConsumeCachedMatchInfo()
{
	FMatchInfo RetData = CachedMatchInfo.Get(FMatchInfo());
	CachedMatchInfo.Reset();
	return RetData;
}
