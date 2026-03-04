// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPAttachmentData.generated.h"

struct FGameplayTag;
class AAttachmentBase;

/**
 * 부착물 정보를 저장하는 Data Asset
 */
UCLASS()
class FLASHPOINT_API UFPAttachmentData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// AssetManager에서 AttachmentTypeTag에 해당하는 데이터 애셋을 반환하는 헬퍼 함수
	static UFPAttachmentData* Get(const FGameplayTag& AttachmentTypeTag);
	
	// ============================================================================
	// Attach
	// ============================================================================

	// 생성할 부착물 액터 클래스
	UPROPERTY(EditDefaultsOnly, Category="Attach")
	TSubclassOf<AAttachmentBase> AttachmentClass;
	
	// 부착물 액터를 부착할 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category="Attach")
	FName AttachSocketName;

	// 부착물 액터를 부착할 때 적용할 Relative Transform
	UPROPERTY(EditDefaultsOnly, Category="Attach")
	FTransform AttachTransform;
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
