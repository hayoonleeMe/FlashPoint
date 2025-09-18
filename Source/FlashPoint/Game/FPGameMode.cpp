// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameMode.h"

#include "EngineUtils.h"
#include "FPLogChannels.h"
#include "Character/FPCharacter.h"
#include "Player/FPPlayerController.h"
#include "Player/FPPlayerState.h"
#include "Player/TeamPlayerStart.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameMode)

AFPGameMode::AFPGameMode()
{
	DefaultPawnClass = AFPCharacter::StaticClass();
	PlayerControllerClass = AFPPlayerController::StaticClass();
	PlayerStateClass = AFPPlayerState::StaticClass();
}

AActor* AFPGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (Player)
	{
		if (TeamPlayerStartsMap.IsEmpty())
		{
			InitializeTeamPlayerStarts();
		}
		
		if (ABasePlayerState* BasePS = Player->GetPlayerState<ABasePlayerState>())
		{
			// TDM과 FFA 모두 동일한 로직으로 처리 (FFA는 Team이 None)
			ETeam PlayerTeam = BasePS->GetTeam();
			if (const FTeamPlayerStartArray* TeamPlayerStartArray = TeamPlayerStartsMap.Find(PlayerTeam))
			{
				const TArray<TObjectPtr<ATeamPlayerStart>>& TeamPlayerStarts = TeamPlayerStartArray->TeamPlayerStarts;
				if (!TeamPlayerStarts.IsEmpty())
				{
					const int32 RandIndex = FMath::RandRange(0, TeamPlayerStarts.Num() - 1);
					return TeamPlayerStarts[RandIndex];
				}
			}
		}
	}
	
	UE_LOG(LogFP, Error, TEXT("[%hs] Can't find a proper player start. Falling back to AGameModeBase::ChoosePlayerStart()"), __FUNCTION__);
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AFPGameMode::InitializeTeamPlayerStarts()
{
	// 팀 별로 저장
	for (TActorIterator<ATeamPlayerStart> It(GetWorld()); It; ++It)
	{
		if (It)
		{
			FTeamPlayerStartArray& TeamPlayerStartArray = TeamPlayerStartsMap.FindOrAdd(It->GetTeam());
			TeamPlayerStartArray.TeamPlayerStarts.Add(*It);
		}
	}
}
