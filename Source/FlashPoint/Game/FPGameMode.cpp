// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameMode.h"

#include "EngineUtils.h"
#include "FPGameplayTags.h"
#include "FPGameState.h"
#include "FPLogChannels.h"
#include "Character/FPCharacter.h"
#include "Player/FPPlayerController.h"
#include "Player/FPPlayerState.h"
#include "Player/TeamPlayerStart.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameMode)

AFPGameMode::AFPGameMode()
{
	DefaultPawnClass = AFPCharacter::StaticClass();
	PlayerControllerClass = AFPPlayerController::StaticClass();
	PlayerStateClass = AFPPlayerState::StaticClass();
	GameStateClass = AFPGameState::StaticClass();

	MatchTime = 600;
	MatchEndDelay = 12.f;
	MatchEndTimeDilation = 0.2f;
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

void AFPGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// MatchTime만큼 시간이 지나면 매치를 종료하도록 타이머 설정
	// EndMatch()가 호출되면 GameMode, GameState의 HandleMatchHasEnded()가 호출된다.
	FTimerHandle MatchTimer;
	GetWorldTimerManager().SetTimer(MatchTimer, FTimerDelegate::CreateUObject(this, &ThisClass::EndMatch), MatchTime, false);

	if (AFPGameState* FPGameState = GetGameState<AFPGameState>())
	{
		if (UWorld* World = GetWorld())
		{
			// 클라에서 타이머 표시를 위해 MatchEndTime 설정
			FPGameState->SetMatchEndTime(World->GetTimeSeconds() + MatchTime);
		}
	}
}

void AFPGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	if (AFPGameState* FPGameState = GetGameState<AFPGameState>())
	{
		FPGameState->SetGlobalTimeDilation(MatchEndTimeDilation);
	}

	// Time Dilation에 의해 Timer Rate도 영향을 받으므로, 이를 반영해 Rate 설정
	FTimerHandle MatchTimer;
	GetWorldTimerManager().SetTimer(MatchTimer, FTimerDelegate::CreateUObject(this, &ThisClass::TravelToLobby), MatchEndDelay * MatchEndTimeDilation, false);
}

void AFPGameMode::TravelToLobby() const
{
	// 매치에서 로비로 Seamless Travel을 수행하기 전, MatchInfo 데이터 캐싱
	CacheMatchInfo();
	
	if (UWorld* World = GetWorld())
	{
		const FString LobbyLevelPath = UFPAssetManager::GetAssetPathByTag(FPGameplayTags::Asset::Level::Lobby).GetAssetName();
		World->ServerTravel(LobbyLevelPath);
	}
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
