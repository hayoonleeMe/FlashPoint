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

protected:
	virtual void BeginPlay() override;

	// 플레이어의 초기 Input Mode를 설정하는 가상함수
	virtual void SetInitialInputMode();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UUIManageComponent> UIManageComponent;
};
