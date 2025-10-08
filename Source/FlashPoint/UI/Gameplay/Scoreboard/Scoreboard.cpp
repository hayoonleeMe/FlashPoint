// Fill out your copyright notice in the Description page of Project Settings.


#include "Scoreboard.h"

#include "Game/BaseGameState.h"
#include "Player/BasePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Scoreboard)

void UScoreboard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ABasePlayerState* BasePS = GetOwningPlayerState<ABasePlayerState>())
	{
		PlayerUsername = BasePS->GetUsername();
	}

	if (GetWorld())
	{
		if (ABaseGameState* BaseGS = GetWorld()->GetGameState<ABaseGameState>())
		{
			BaseGS->OnClientPlayerInfoAddedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoAdded);
			BaseGS->OnClientPlayerInfoRemovedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoRemoved);
			BaseGS->OnClientPlayerInfoChangedDelegate.AddUObject(this, &ThisClass::OnClientPlayerInfoChanged);
		}
	}
}
