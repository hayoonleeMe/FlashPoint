// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/MatchTypes.h"
#include "Engine/GameInstance.h"
#include "FPGameInstance.generated.h"

/**
 * Base Game Instance
 */
UCLASS()
class FLASHPOINT_API UFPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	// Seamless Travel 중 데이터를 유지하기 위해 캐싱
	void CacheMatchInfo(const FMatchInfo& InMatchInfo);

	// Seamless Travel이 끝난 후, 데이터를 복구하고 비움
	FMatchInfo ConsumeCachedMatchInfo();

private:
	// Seamless Travel 중 임시로 보관할 데이터
	TOptional<FMatchInfo> CachedMatchInfo;
};
