// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerListRow.generated.h"

class UTextBlock;

/**
 * 로비의 플레이어 리스트에 추가할 Row Widget
 */
UCLASS()
class FLASHPOINT_API UPlayerListRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetUsername(const FString& InUsername);
	FString GetUsername() const { return Username; }

	void HighlightUsername() const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Username;

	FString Username;

	// 현재 Row가 플레이어 본인일 때, 텍스트 강조 표시 색상
	static constexpr FLinearColor HighlightColor{ 1.f, 0.9f, 0.26f, 1.f }; 
};
