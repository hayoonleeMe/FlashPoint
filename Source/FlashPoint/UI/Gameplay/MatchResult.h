// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MatchTypes.h"
#include "MatchResult.generated.h"

class UTextBlock;

/**
 * 매치 결과를 표시하는 Widget Class
 */
UCLASS()
class FLASHPOINT_API UMatchResult : public UUserWidget
{
	GENERATED_BODY()

public:
	UMatchResult(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeOnInitialized() override;

private:
	void InitializeTDM(ETeam UserTeam, ETeam WinningTeam);
	void InitializeFFA(int32 UserRank);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Result;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeIn;

	UPROPERTY(EditDefaultsOnly)
	FString VictoryText;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor VictoryColor;

	UPROPERTY(EditDefaultsOnly)
	FString DefeatText;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor DefeatColor;

	UPROPERTY(EditDefaultsOnly)
	FString DrawText;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor DrawColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor DefaultRankColor;
};
