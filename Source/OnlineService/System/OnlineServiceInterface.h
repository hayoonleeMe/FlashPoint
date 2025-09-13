// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OnlineServiceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UOnlineServiceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ONLINESERVICE_API IOnlineServiceInterface
{
	GENERATED_BODY()

public:
	virtual void RefreshTokens(const FString& RefreshToken) = 0;
};
