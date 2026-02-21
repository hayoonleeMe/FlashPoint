// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPWeaponConfigData.generated.h"

/**
 * 무기 데이터를 저장하는 Data Asset
 */
UCLASS()
class FLASHPOINT_API UFPWeaponConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 한 번에 발사되는 총알 수 
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	int32 BulletsPerCartridge;

	// 한 탄창 당 총알 수
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	int32 MagCapacity;

	// 최대 사거리
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	float MaxDamageRange;

	// 기본 데미지
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	float BaseDamage;

	// 거리 비례 데미지 감소 커브
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	TObjectPtr<UCurveFloat> DamageFallOffCurve;

	// 헤드샷 데미지 배율
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	float HeadShotMultiplier;

	// 왼손을 부착할 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	FName LeftHandAttachSocketName;
	
	// HUD에 표시할 무기 이름
	UPROPERTY(EditDefaultsOnly, Category="UI")
	FString DisplayName;

	// HUD에 표시할 Icon
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TObjectPtr<UTexture2D> DisplayIcon;
};
