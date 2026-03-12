// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "FPCameraComponent.generated.h"

/**
 * 추가적인 오프셋을 적용하기 위한 View Data
 */
struct FCameraOffsetView
{
	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	float FOV = 0.f;
	
	void Blend(const FCameraOffsetView& Other, float OtherWeight);
};

/**
 * 카메라에 적용할 오프셋을 정의하는 오브젝트 클래스
 */
UCLASS(Blueprintable)
class FLASHPOINT_API UFPCameraOffset : public UObject
{
	GENERATED_BODY()
	
public:
	UFPCameraOffset();
	
	const FCameraOffsetView& GetOffsetView() const { return OffsetView; }
	
	float GetBlendTime() const { return BlendTime; }
	float GetBlendWeight() const { return BlendWeight; }
	void SetBlendWeight(float Weight);
	
	void UpdateCameraOffset(float DeltaTime, FCameraOffsetView& OutOffsetView);
	
protected:
	virtual void UpdateOffsetView(float DeltaTime);
	virtual void UpdateBlending(float DeltaTime);
	
	// Camera Offset 오브젝트에 의해 생성된 Offset View
	FCameraOffsetView OffsetView;
	
	UPROPERTY(EditDefaultsOnly, Category="Offset")
	FVector OffsetLocation;
	
	UPROPERTY(EditDefaultsOnly, Category="Offset")
	FRotator OffsetRotation;
	
	UPROPERTY(EditDefaultsOnly, Category="Offset")
	float OffsetFOV;
	
	// todo : add curve option?
	
	// Offset이 완전히 적용될 때까지 걸리는 시간
	UPROPERTY(EditDefaultsOnly, Category="Blend")
	float BlendTime;
	
	// todo : blend function, blend exponent?
	
	// BlendWeight를 계산하기 위한 Linear Alpha
	float BlendAlpha;
	
	// BlendAlpha로 계산된 최종 Weight
	float BlendWeight;
};

/**
 * Camera Offset 기능이 추가된 카메라 컴포넌트 클래스
 */
UCLASS()
class FLASHPOINT_API UFPCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public:
	UFPCameraComponent();

	virtual void Deactivate() override;
	
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	
	// 적용할 Camera Offset 오브젝트 클래스를 가져올 때 사용할 델레게이트
	DECLARE_DELEGATE_RetVal(TSubclassOf<UFPCameraOffset>, FDetermineCameraOffsetDelegate);
	// 델레게이트에 등록된 함수가 제공하는 Camera Offset 오브젝트 클래스를 적용한다.
	FDetermineCameraOffsetDelegate DetermineCameraOffsetDelegate;
	
protected:
	// 적용할 Camera Offset을 결정한다.
	virtual void DetermineCameraOffsets();
	
	// 등록된 Camera Offset을 적용한다.
	void UpdateCameraOffsets(float DeltaTime, FCameraOffsetView& OutOffsetView);
	
	// CameraOffsetClass 오브젝트 인스턴스를 반환한다.
	// 캐싱되어 있지 않으면 생성하고 캐싱한다.
	UFPCameraOffset* GetCameraOffsetInstance(TSubclassOf<UFPCameraOffset> CameraOffsetClass);
	
	// 현재 적용 중인 Camera Offset
	UPROPERTY()
	TObjectPtr<UFPCameraOffset> MainCameraOffset;
	
	// 이전에 적용한 Camera Offset
	UPROPERTY()
	TObjectPtr<UFPCameraOffset> PendingCameraOffset;
	
	// 캐시된 UFPCameraOffset 인스턴스
	UPROPERTY()
	TArray<TObjectPtr<UFPCameraOffset>> CachedCameraOffsetInstances;
};
