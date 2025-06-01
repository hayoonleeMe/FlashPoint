// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FLASHPOINT_API AFPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFPPlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
