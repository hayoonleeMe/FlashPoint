// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardRow.generated.h"

class UTextBlock;

/**
 * Scoreboard Widget의 Row Widget
 */
UCLASS()
class FLASHPOINT_API UScoreboardRow : public UUserWidget
{
	GENERATED_BODY()

public:
	// 파라미터에 따라 해당 위젯을 설정하고 화면에 표시한다.
	void SetRow(const FString& Username, int32 KillCount, int32 DeathCount, bool bIsLocalPlayer) const;
	
	// bShow에 따라 해당 위젯을 화면에 표시하거나 숨긴다.
	void ShowRow(bool bShow) const;

	// 해당 위젯이 화면에 표시 중인지 반환한다.
	// 해당 위젯은 자체 Visibility를 변경하지 않고 내부 TextBlock의 Visibility만 변경하므로, 이 함수를 통해 표시 유무를 체크한다.
	bool IsRowVisible() const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Username;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Kill;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Death;

	// 현재 Row가 플레이어 본인일 때, 텍스트 강조 표시 색상
	static constexpr FLinearColor HighlightColor{ 1.f, 0.9f, 0.26f, 1.f };
};
