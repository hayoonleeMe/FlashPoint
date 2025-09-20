// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UUIManageComponent;

/**
 * Main Widget을 표시하고, Initial Input Mode를 설정하는 Base Player Controller
 */
UCLASS()
class FLASHPOINT_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();

	// UI Input Mode로 설정한다.
	virtual void SetUIInputMode();

	// Gameplay Input Mode로 설정한다.
	virtual void SetGameplayInputMode();
	
protected:
	virtual void BeginPlay() override;
	
	// BeginPlay()에서 호출되어 플레이어의 초기 Input Mode를 설정하는 가상함수
	virtual void SetInitialInputMode();

	virtual void SetupInputComponent() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UUIManageComponent> UIManageComponent;

private:
	void Input_UI_Back();
	void Input_UI_Confirm();
};
