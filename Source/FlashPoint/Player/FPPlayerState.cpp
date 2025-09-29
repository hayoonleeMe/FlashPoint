// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerState.h"

#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FPAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPPlayerState)

AFPPlayerState::AFPPlayerState()
{
	NetUpdateFrequency = 120.f;
	MinNetUpdateFrequency = 60.f;
	
	AbilitySystemComponent = CreateDefaultSubobject<UFPAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UFPAttributeSet>(TEXT("AttributeSet"));
}

void AFPPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPPlayerState, KillCount);
	DOREPLIFETIME(AFPPlayerState, DeathCount);
}

UAbilitySystemComponent* AFPPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAbilitySystemComponent* AFPPlayerState::GetFPAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAttributeSet* AFPPlayerState::GetFPAttributeSet() const
{
	return AttributeSet;
}

void AFPPlayerState::AddToKillCount(int32 AddAmount)
{
	KillCount += AddAmount;
	OnPlayerKillCountChangedDelegate.Broadcast(Team, AddAmount);
}

void AFPPlayerState::AddToDeathCount(int32 AddAmount)
{
	DeathCount += AddAmount;
}

FPlayerInfo AFPPlayerState::MakePlayerInfo() const
{
	FPlayerInfo Ret = Super::MakePlayerInfo();
	Ret.KillCount = KillCount;
	Ret.DeathCount = DeathCount;
	return Ret;
}

void AFPPlayerState::OnRep_KillCount(int32 OldKillCount)
{
	OnPlayerKillCountChangedDelegate.Broadcast(Team, KillCount - OldKillCount);
}
