// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "GameplayTagContainer.h"
#include "Data/MatchTypes.h"
#include "FPPlayerController.generated.h"

class UScoreboard;
class UPauseMenu;
class UFPAbilitySystemComponent;
struct FInputActionValue;

/**
 * 게임플레이에서 사용할 PlayerController
 */
UCLASS()
class FLASHPOINT_API AFPPlayerController : public ABasePlayerController
{
	GENERATED_BODY()

public:
	virtual void SetUIInputMode() override;
	virtual void SetGameplayInputMode() override;
	
protected:
	virtual void SetInitialInputMode() override;
	virtual void SetupInputComponent() override;

private:
	void Input_Move(const FInputActionValue& InputValue);
	void Input_Look(const FInputActionValue& InputValue);
	void Input_Crouch();
	void Input_PauseMenu();
	void Input_Scoreboard(const FInputActionValue& InputValue);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	UFPAbilitySystemComponent* GetFPAbilitySystemComponent() const;

	// ============================================================================
	// UI
	// ============================================================================
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UPauseMenu> PauseMenuClass;

	// 클라이언트로 MatchInfo 프로퍼티가 Replicate될 때 호출되는 Callback
	void OnClientMatchInfoReplicated(const FMatchInfo& MatchInfo);

	// Cached Scoreboard Widget
	UPROPERTY()
	TObjectPtr<UScoreboard> Scoreboard;

	// MatchMode 별 Scoreboard Widget Class
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TMap<EMatchMode, TSubclassOf<UScoreboard>> ScoreboardClasses;
};
