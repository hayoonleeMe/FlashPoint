// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchTimer.generated.h"

class UTextBlock;

/**
 * 매치의 남은 시간을 표시하는 위젯
 */
UCLASS()
class FLASHPOINT_API UMatchTimer : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativePreConstruct() override;

private:
	void OnClientMatchEndTimeReplicated(float InMatchEndTime);

	UPROPERTY()
	TWeakObjectPtr<AGameStateBase> CachedGameState;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Time;

	// GameState에서 전달한 값으로 설정된다.
	float MatchEndTime = 0.f;
	
	// 최근 업데이트된 MatchEndTime까지 남은 시간(초)
	int32 LastSeconds = 0;
};
