// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "APIData.generated.h"

/**
 * API 호출을 위한 정보를 저장하는 Data Asset
 */
UCLASS()
class ONLINESERVICE_API UAPIData : public UDataAsset
{
	GENERATED_BODY()

public:
	FString GetAPIEndpoint(const FGameplayTag& APIEndpoint);

private:
	// API를 식별하기 위한 이름
	UPROPERTY(EditDefaultsOnly)
	FString Name;

	UPROPERTY(EditDefaultsOnly)
	FString InvokeURL;

	UPROPERTY(EditDefaultsOnly)
	FString Stage;
	
	UPROPERTY(EditDefaultsOnly, meta=(Categories="OnlineServiceTags"))
	TMap<FGameplayTag, FString> Resources;
};
