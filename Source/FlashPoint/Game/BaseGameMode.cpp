// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "FPGameplayTags.h"
#include "BaseGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BasePlayerState.h"
#include "System/FPAssetManager.h"
#include "System/FPGameInstance.h"
#include "System/OnlineServiceSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseGameMode)

ABaseGameMode::ABaseGameMode()
{
	GameStateClass = ABaseGameState::StaticClass();
	PlayerStateClass = ABasePlayerState::StaticClass();
	bUseSeamlessTravel = true;
}

void ABaseGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	// 해당 서버로 접속할 때 사용한 URL의 옵션 데이터를 가져온다.
	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

	// GameLift GameSession에 PlayerSession을 등록한다.
	// ErrorMessage가 설정되면 이 로그인은 실패한다.
	UOnlineServiceSubsystem::TryAcceptPlayerSession(PlayerSessionId, Username, ErrorMessage);
}

FString ABaseGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	// PreLogin()이 성공적으로 수행되어 Player Session이 Accept 된 후, 플레이어의 NewPlayerController가 생성된 후에 호출된다.
	// 플레이어가 해당 서버로 클라이언트로 들어오는 경우에만 호출된다.
	// 이 시점에서 PlayerController와 PlayerState는 모두 유효
	check(NewPlayerController);

	// 해당 서버로 접속할 때 사용한 URL의 옵션 데이터를 가져온다.
	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));
	
	if (ABasePlayerState* BasePS = NewPlayerController->GetPlayerState<ABasePlayerState>())
	{
		// 추후 PlayerSession을 제거하기 위해 저장
		BasePS->SetServerPlayerSessionId(PlayerSessionId);
		BasePS->SetServerUsername(Username);

		// 플레이어의 팀이 변경될 때 PlayerInfoArray와 위젯을 업데이트하도록 등록
		BasePS->OnServerPlayerTeamChangedDelegate.AddUObject(this, &ThisClass::OnPlayerTeamChanged);
		
		if (ABaseGameState* BaseGS = GetGameState<ABaseGameState>())
		{
			// TDM이라면 플레이어의 팀을 선택하고, 배열에 추가해 관리
			ChoosePlayerTeam(BasePS);
			BaseGS->AddPlayerInfo(BasePS->MakePlayerInfo());
		}
	}
	
	// 부모 클래스에서 검사
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	if (ErrorMessage.IsEmpty())
	{
		// 로그인에 성공했을 때 접속한 플레이어 수를 업데이트
		++MatchInfo.CurrentPlayers;
	}
		
	return ErrorMessage;
}

void ABaseGameMode::Logout(AController* Exiting)
{
	if (Exiting)
	{
		if (ABasePlayerState* BasePS = Exiting->GetPlayerState<ABasePlayerState>())
		{
			if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
			{
				// GameLift GameSession에 등록한 PlayerSession을 제거
				OnlineServiceSubsystem->RemovePlayerSession(BasePS->GetServerPlayerSessionId());

				if (ABaseGameState* BaseGS = GetGameState<ABaseGameState>())
				{
					if (MatchInfo.MatchMode == EMatchMode::TeamDeathMatch)
					{
						RedTeamPlayers.Remove(BasePS);
						BlueTeamPlayers.Remove(BasePS);
					}

					// 배열에서 제거
					BaseGS->RemovePlayerInfo(BasePS->GetServerUsername());
				}

				const bool bIsHost = MatchInfo.HostId == BasePS->GetServerUsername();
				if (bIsHost)
				{
					// 방장이 나가면 GameSession 제거
					// 이후 모든 플레이어 추방
					OnlineServiceSubsystem->TerminateGameSession(MatchInfo.GameSessionId);
				}
			}
		}	
	}

	Super::Logout(Exiting);
}

void ABaseGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	// 로비 <-> 매치 Seamless Travel이 끝나면 호출되고, 데이터를 복구한다.
	
	// MatchInfo 데이터 복구
	if (UFPGameInstance* FPGameInstance = GetGameInstance<UFPGameInstance>())
	{
		MatchInfo = FPGameInstance->ConsumeCachedMatchInfo();

		if (ABaseGameState* BaseGS = GetGameState<ABaseGameState>())
		{
			BaseGS->SetMatchInfo(MatchInfo);
		}
	}

	// ABaseGameState::PlayerInfoArray 유지
	if (ABaseGameState* BaseGS = GetGameState<ABaseGameState>())
	{
		for (APlayerState* PS : BaseGS->PlayerArray)
		{
			if (ABasePlayerState* BasePS = Cast<ABasePlayerState>(PS))
			{
				BaseGS->AddPlayerInfo(BasePS->MakePlayerInfo());

				// 팀 정보 유지
				if (BasePS->GetTeam() == ETeam::RedTeam)
				{
					RedTeamPlayers.Add(BasePS);
				}
				else if (BasePS->GetTeam() == ETeam::BlueTeam)
				{
					BlueTeamPlayers.Add(BasePS);
				}
			}
		}
	}
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UOnlineServiceSubsystem* OnlineServiceSubsystem = UOnlineServiceSubsystem::Get(this))
	{
		if (!OnlineServiceSubsystem->OnServerStartGameSessionDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->OnServerStartGameSessionDelegate.AddUObject(this, &ThisClass::OnGameSessionStarted);
		}
		if (!OnlineServiceSubsystem->OnProcessTerminateDelegate.IsBoundToObject(this))
		{
			OnlineServiceSubsystem->OnProcessTerminateDelegate.AddUObject(this, &ThisClass::OnGameSessionTerminated);
		}
		OnlineServiceSubsystem->InitGameLift();
	}
}

void ABaseGameMode::ChoosePlayerTeam(ABasePlayerState* BasePS)
{
	if (MatchInfo.MatchMode == EMatchMode::TeamDeathMatch)
	{
		const int32 NumRedTeam = RedTeamPlayers.Num();
		const int32 NumBlueTeam = BlueTeamPlayers.Num();
		if (NumRedTeam <= NumBlueTeam)
		{
			BasePS->SetTeam(ETeam::RedTeam);
			RedTeamPlayers.Add(BasePS);
		}
		else
		{
			BasePS->SetTeam(ETeam::BlueTeam);
			BlueTeamPlayers.Add(BasePS);
		}
	}
}

void ABaseGameMode::OnPlayerTeamChanged(ABasePlayerState* BasePS)
{
	if (ABaseGameState* BaseGS = GetGameState<ABaseGameState>())
	{
		BaseGS->UpdatePlayerInfo(BasePS->MakePlayerInfo());
	}
}

void ABaseGameMode::OnGameSessionStarted(const FGameSessionInfo& GameSessionInfo)
{
	MatchInfo = FMatchInfo(GameSessionInfo);
	MatchInfo.Dump();

	if (ABaseGameState* BaseGS = GetGameState<ABaseGameState>())
	{
		BaseGS->SetMatchInfo(MatchInfo);
	}
}

void ABaseGameMode::OnGameSessionTerminated()
{
	AsyncTask(ENamedThreads::Type::GameThread, [this]()
	{
		// GameThread에서, 서버에 접속한 모든 플레이어 추방
		KickAllPlayers();
	});
}

void ABaseGameMode::KickAllPlayers() const
{
	if (GetWorld())
	{
		const TCHAR* KickReason = IsHostStillRemain() ? SessionTerminatedKickReason : HostLogoutKickReason;

		const FString MainMenuLevelPath = UFPAssetManager::GetAssetPathByTag(FPGameplayTags::Asset::Level::MainMenu).GetAssetName();
		const FString URL = FString::Printf(TEXT("%s?%s=%s"), *MainMenuLevelPath, KickReasonOption, KickReason);

		for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				// ClientTravel()은 내부에서 Client RPC로 동작
				PC->ClientTravel(URL, TRAVEL_Absolute);
			}
		}
	}
}

bool ABaseGameMode::IsHostStillRemain() const
{
	for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (ABasePlayerState* BasePS = PC->GetPlayerState<ABasePlayerState>())
			{
				// Host Still Remain
				if (BasePS->GetServerUsername() == MatchInfo.HostId)
				{
					return true;
				}
			}
		}
	}
	return false;
}
