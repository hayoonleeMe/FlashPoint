// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseGameState.h"
#include "FPGameState.generated.h"

/**
 * 게임플레이에서 사용할 GameState
 */
UCLASS()
class FLASHPOINT_API AFPGameState : public ABaseGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// GameMode에서 전달한 MatchEndTime을 설정한다.
	void SetMatchEndTime(float InMatchEndTime);

private:
	// 매치가 종료되는 WorldTimeSeconds을 나타낸다.
	// 서버에서 매치를 시작한 뒤 설정된다.
	UPROPERTY(ReplicatedUsing=OnRep_MatchEndTime)
	float MatchEndTime;
	
	UFUNCTION()
	void OnRep_MatchEndTime();
};
