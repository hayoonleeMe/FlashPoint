// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMaterialComponent.h"
#include "CustomFovComponent.generated.h"

/**
 * 등록한 메시에 카메라와 별개인 Custom FOV를 적용하는 컴포넌트
 * MF_CustomFOV에서 사용하는 파라미터를 관리한다.
 * 
 */
UCLASS()
class FLASHPOINT_API UCustomFovComponent : public UDynamicMaterialComponent
{
	GENERATED_BODY()
	
public:
	UCustomFovComponent();
	
	// 새로운 컴포넌트를 생성하고 등록한 뒤 반환한다.
	static UCustomFovComponent* CreateComponent(
		UObject* Outer,
		const FName& Name,
		UAbilitySystemComponent* ASC,
		const TArray<UMeshComponent*>& MeshesToRegister
	);
	
	// CustomFOV 파라미터 값을 설정한다.
	void SetCustomFOV(float Value);
	
	// EnableCustomFOV 파라미터 값을 설정한다.
	void SetEnableCustomFOV(float Value);
	
	// ScaleInDepth 파라미터 값을 설정한다.
	void SetScaleInDepth(float Value);
	
	// ScreenOffset 파라미터 값을 설정한다.
	void SetScreenOffset(const FVector& Value);
};
