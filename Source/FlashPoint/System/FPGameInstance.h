﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FPGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FLASHPOINT_API UFPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
