// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Curves/CurveFloat.h"
#include "FPRecoilData.generated.h"

/**
 * 반동에 대한 정보를 저장하는 DataAsset
 *
 * DA_AssetData에 Asset.RecoilData GameplayTag로 무기별로 저장된다.
 */
UCLASS(BlueprintType)
class FLASHPOINT_API UFPRecoilData : public UDataAsset
{
	GENERATED_BODY()

	// Key: Weapon.Type, Value: Asset.RecoilData
	static TMap<FGameplayTag, FGameplayTag> WeaponTypeToRecoilDataTagMap;
	
public:
	// WeaponTypeTag에 해당하는 RecoilData Tag를 반환한다.
	static FGameplayTag GetRecoilDataTagByWeaponType(const FGameplayTag& WeaponTypeTag);
	
	// 수직 반동 세기 커브 (X: 누적 발사 수, Y: 세기)
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	TObjectPtr<UCurveFloat> VerticalRecoilCurve;

	// 수평 반동 세기 커브 (X: 누적 발사 수, Y: 세기)
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	TObjectPtr<UCurveFloat> HorizontalRecoilCurve;

	// 반동이 목표치까지 도달하는 보간 속도
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilInterpSpeed;

	// 발사 중단 후, 반동 패턴이 초기화되고 회복이 시작될 때까지의 딜레이
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoveryDelay;

	// 무기 장착 시 기본적으로 조준점이 벌어지는 정도
	UPROPERTY(EditDefaultsOnly, Category = "Aim Spread")
	float AimSpreadWeaponOffset;
	
	// 발사 시마다 조준점이 벌어지는 정도
	UPROPERTY(EditDefaultsOnly, Category = "Aim Spread")
	float AimSpreadIncrease;

	// 조준점이 최대로 벌어질 수 있는 크기
	UPROPERTY(EditDefaultsOnly, Category = "Aim Spread")
	float MaxAimSpread;

	// 조준점이 정상 크기로 돌아오는 회복 속도
	UPROPERTY(EditDefaultsOnly, Category = "Aim Spread")
	float AimSpreadRecoverySpeed;
};
