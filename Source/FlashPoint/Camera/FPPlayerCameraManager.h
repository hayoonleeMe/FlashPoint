// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "FPPlayerCameraManager.generated.h"

/**
 * 기본으로 사용할 PlayerCameraManager
 */
UCLASS()
class FLASHPOINT_API AFPPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AFPPlayerCameraManager();
};
