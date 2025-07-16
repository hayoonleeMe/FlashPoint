// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FPCosmeticData.generated.h"

/**
 * Animation Layer와 선택에 필요한 태그를 저장
 */
USTRUCT(BlueprintType)
struct FAnimLayerEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> AnimLayer;

	// Anim Layer를 선택할 때 필요한 태그
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer RequiredTags;
};

/**
 * Animation Layer 항목들을 저장
 */
USTRUCT(BlueprintType)
struct FAnimLayerSet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FAnimLayerEntry> LayerEntries;

	// LayerEntries에서 선택하지 못할 경우 사용할 기본 Anim Layer
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> DefaultAnimLayer;
};

/**
 * Cosmetic Data를 저장하는 DataAsset
 */
UCLASS()
class FLASHPOINT_API UFPCosmeticData : public UDataAsset
{
	GENERATED_BODY()

public:
	// CosmeticTags에 포함된 태그에 해당하는 Anim Layer를 선택해 반환한다.
	TSubclassOf<UAnimInstance> SelectAnimLayer(const FGameplayTagContainer& CosmeticTags) const;

	// DefaultAnimLayer를 반환한다.
	TSubclassOf<UAnimInstance> GetDefaultAnimLayer() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	FAnimLayerSet AnimLayerSet;
};
