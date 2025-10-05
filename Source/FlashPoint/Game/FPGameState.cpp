// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameState.h"

#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameState)

void AFPGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPGameState, MatchEndTime);
}

void AFPGameState::SetMatchEndTime(float InMatchEndTime)
{
	MatchEndTime = InMatchEndTime;
}

void AFPGameState::OnRep_MatchEndTime()
{
}
