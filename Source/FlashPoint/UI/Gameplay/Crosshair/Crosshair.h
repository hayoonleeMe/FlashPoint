// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Crosshair.generated.h"

class UImage;

/**
 * 에임 퍼짐 정도에 반응하는 크로스헤어 위젯
 */
UCLASS()
class FLASHPOINT_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()

public:
	UCrosshair(const FObjectInitializer& ObjectInitializer);
	virtual void UpdateCrosshair(float AimSpread) { }

protected:
	// Crosshair의 벌어지는 정도를 결정하는 AimSpread 값에 곱해질 값
	UPROPERTY(EditDefaultsOnly)
	float CrosshairSpreadScale;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Center;
};
