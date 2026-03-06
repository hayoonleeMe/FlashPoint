// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMaterialComponent.h"
#include "ScopeLensMaskComponent.generated.h"

/**
 * 등록한 메시에 스코프 렌즈를 기준으로 일정 Radius만큼 투명 마스크를 적용하는 컴포넌트
 * MF_ScopeLensMask에서 사용하는 파라미터를 관리한다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UScopeLensMaskComponent : public UDynamicMaterialComponent
{
	GENERATED_BODY()

public:
	UScopeLensMaskComponent();
	
	// 새로운 컴포넌트를 생성하고 등록한 뒤 반환한다.
	static UScopeLensMaskComponent* CreateComponent(
		UObject* Outer,
		const FName& Name,
		UAbilitySystemComponent* ASC,
		const TArray<UMeshComponent*>& MeshesToRegister
	);
	
	// EnableScopeLensMask 파라미터 값을 설정한다.
	UFUNCTION(BlueprintCallable)
	void SetEnableScopeLensMask(float Value);
	
	// LensRadius 파라미터 값을 설정한다.
	UFUNCTION(BlueprintCallable)
	void SetLensRadius(float Value);
	
	// LocalLensLocation 파라미터 값을 설정한다.
	UFUNCTION(BlueprintCallable)
	void SetLocalLensLocation(const FVector& Value);
};
