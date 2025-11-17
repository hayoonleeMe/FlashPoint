// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponWallTraceComponent.generated.h"

class AWeapon_Base;

/**
 * 벽 등의 장애물에 의해 무기를 내리거나 올리는 액터 컴포넌트
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UWeaponWallTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponWallTraceComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Weapon Block 데이터들을 반환한다.
	void RetrieveWeaponWallBlockData(bool& bOutIsWeaponWallBlocked, bool& bOutUseWeaponUp, FVector& OutLocalWallHitLocation) const;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<AWeapon_Base> EquippedWeapon;

	void UpdateEquippedWeapon(AWeapon_Base* InEquippedWeapon);

	// Wall Trace를 수행하고 Weapon Block 데이터를 계산한다.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 로컬 기기에서 Wall Trace를 수행한다.
	void LocalWallTrace(const APawn* Pawn);

	// 로컬에서 Wall Trace가 성공하면 저장되는 벽의 위치
	FVector LocalWallHitLocation;

	// 벽에 대해 Sphere Trace를 수행할 Sphere Radius
	UPROPERTY(EditDefaultsOnly)
	float WallTraceRadius;

	// Weapon Block 시 Up Pose를 사용할 Pitch 임계값
	UPROPERTY(EditDefaultsOnly)
	float WeaponUpPitchThreshold;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> WallTraceObjectTypes;

	// Wall Trace 시작 지점 계산 시, 상하 오프셋
	// +: Up, -: Down
	UPROPERTY(EditDefaultsOnly)
	float WallTraceStartUpOffset;

	// Wall Trace 시작 지점 계산 시, 우측 방향 오프셋
	// +: Right, -: Left
	UPROPERTY(EditDefaultsOnly)
	float WallTraceStartRightOffset;

	// Wall Trace 시작 지점 계산 시, AimPitch에 따른 전방 방향 오프셋 범위
	// +: Forward, -: Backward
	UPROPERTY(EditDefaultsOnly)
	FVector2D WallTraceStartForwardOffsetOutRange;

	// Wall Trace Length 계산 시, Abs(AimPitch)에 따른 Length Multiplier 범위
	UPROPERTY(EditDefaultsOnly)
	FVector2D WallTraceLengthMultiplierOutRange;
	
	UPROPERTY(Replicated)
	bool bIsWeaponWallBlocked;
	
	void SetIsWeaponWallBlocked(bool bInIsWeaponWallBlocked);
	
	UFUNCTION(Server, Unreliable)
	void ServerSetIsWeaponWallBlocked(bool bInIsWeaponWallBlocked);

	UPROPERTY(Replicated)
	bool bUseWeaponUp;

	void SetUseWeaponUp(bool bInUseWeaponUp);
	
	UFUNCTION(Server, Unreliable)
	void ServerSetUseWeaponUp(bool bInUseWeaponUp);
};
