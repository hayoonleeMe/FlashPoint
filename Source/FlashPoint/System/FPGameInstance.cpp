﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "FPGameInstance.h"

#include "FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPGameInstance)

void UFPGameInstance::Init()
{
	Super::Init();

	UFPAssetManager::Initialize();
}
