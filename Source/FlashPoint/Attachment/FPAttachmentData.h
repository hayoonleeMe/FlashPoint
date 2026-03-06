// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPAttachmentData.generated.h"

struct FGameplayTag;
class AAttachmentBase;

/**
 * 부착물의 스탯을 정의하는 기본 오브젝트 클래스
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UAttachmentStat : public UObject
{
	GENERATED_BODY()
};

/**
 * UpperRail 부착물의 스탯을 정의하는 오브젝트 클래스
 */
UCLASS()
class UAttachmentStat_UpperRail : public UAttachmentStat
{
	GENERATED_BODY()

public:
	// AimDownSight 시 카메라에 적용할 FOV
	UPROPERTY(EditDefaultsOnly)
	float AimDownSightFOV;
	
	// AimDownSight 시 무기 메시에 적용할 FOV
	UPROPERTY(EditDefaultsOnly)
	float AimDownSightWeaponFOV;
	
	// AimDownSight 속도에 영향을 주는 정도
	// e.g. -0.2는 20% 감소
	UPROPERTY(EditDefaultsOnly)
	float AimDownSightSpeedModifier;
};

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

	// ============================================================================
	// Stat
	// ============================================================================

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Stat")
	TObjectPtr<UAttachmentStat> AttachmentStat;
	
	// 특정 타입의 Stat 객체를 반환한다.
	template <class T>
	const T* GetAttachmentStat() const
	{
		return Cast<T>(AttachmentStat);
	}
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
