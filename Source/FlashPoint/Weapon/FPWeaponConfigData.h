// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FPWeaponConfigData.generated.h"

class UFPCameraOffset;
class UNiagaraSystem;

/**
 * 무기 데이터를 저장하는 Data Asset
 */
UCLASS()
class FLASHPOINT_API UFPWeaponConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// AssetManager에서 WeaponTypeTag에 해당하는 데이터 애셋을 반환하는 헬퍼 함수
	static UFPWeaponConfigData* Get(const FGameplayTag& WeaponTypeTag);
	
	UFPWeaponConfigData();
	
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
	
	// AimDownSight 시 걸리는 시간
	UPROPERTY(EditDefaultsOnly, Category="Stat")
	float TimeToADS;

	// 왼손을 부착할 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	FName LeftHandAttachSocketName;
	
	// 1인칭 시점일 때 오른손에 적용할 Location 오프셋
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	FVector FirstPersonRightHandLocOffset;
	
	// 1인칭 시점일 때 오른손에 적용할 Rotation 오프셋
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	FRotator FirstPersonRightHandRotOffset;
	
	// AimDownSight 시 카메라 앞에 위치시킬 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	FName AimDownSightSocketName;
	
	// 1인칭 시점일 때 카메라에 적용할 CameraOffset 오브젝트 클래스
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	TSubclassOf<UFPCameraOffset> FirstPersonCameraOffsetClass;
	
	// 1인칭 시점일 때 기본적으로 무기에 적용할 FOV
	UPROPERTY(EditAnywhere, Category="FOV")
	float FirstPersonDefaultWeaponFOV;
	
	// AimDownSight 시 카메라에 적용할 FOV
	UPROPERTY(EditAnywhere, Category="FOV")
	float AimDownSightFOV;
	
	// AimDownSight 시 무기에 적용할 FOV
	UPROPERTY(EditAnywhere, Category="FOV")
	float AimDownSightWeaponFOV;
	
	// HUD에 표시할 무기 이름
	UPROPERTY(EditDefaultsOnly, Category="UI")
	FString DisplayName;

	// HUD에 표시할 Icon
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TObjectPtr<UTexture2D> DisplayIcon;
	
	// 무기를 장착할 캐릭터 메시의 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category="Equip")
	FName AttachSocketName;
	
	// 무기를 장착할 Relative Transform
	UPROPERTY(EditDefaultsOnly, Category="Equip")
	FTransform AttachTransform;

	UPROPERTY(EditDefaultsOnly, Category="Equip")
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category="Equip")
	TObjectPtr<UAnimMontage> UnEquipMontage;

	// CosmeticData에서 필요한 정보를 쿼리할 태그
	UPROPERTY(EditDefaultsOnly, Category="Equip")
	FGameplayTagContainer CosmeticTags;
	
	// 총알 궤적을 나타낼 NiagaraSystem
	UPROPERTY(EditDefaultsOnly, Category="Fire Effects")
	TObjectPtr<UNiagaraSystem> TracerSystem;
};
