// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

#include "Net/UnrealNetwork.h"

ABasePlayerState::ABasePlayerState()
{
	Team = ETeam::None;
}

void ABasePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePlayerState, Team);
}

void ABasePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	// 레벨 이동 시 데이터 유지
	if (ABasePlayerState* OtherPS = Cast<ABasePlayerState>(PlayerState))
	{
		OtherPS->Team = Team;
		OtherPS->ServerPlayerSessionId = ServerPlayerSessionId;
		OtherPS->ServerUsername = ServerUsername;
	}
}

void ABasePlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	// 플레이어 재접속 시 데이터 유지
	if (ABasePlayerState* OtherPS = Cast<ABasePlayerState>(PlayerState))
	{
		Team = OtherPS->Team;
		ServerPlayerSessionId = OtherPS->ServerPlayerSessionId;
		ServerUsername = OtherPS->ServerUsername;
	}
}

FPlayerInfo ABasePlayerState::MakePlayerInfo() const
{
	FPlayerInfo Ret;
	Ret.Username = ServerUsername;
	Ret.Team = Team;
	return Ret;
}

void ABasePlayerState::SetTeam(ETeam InTeam)
{
	ETeam LastTeam = Team;
	Team = InTeam;
	
	if (HasAuthority() && LastTeam != ETeam::None && LastTeam != Team)
	{
		OnServerPlayerTeamChangedDelegate.Broadcast(this);
	}
}
