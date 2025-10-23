// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crosshair.h"
#include "ShotgunCrosshair.generated.h"

/**
 * Shotgun 장착 시 사용할 Crosshair Widget
 */
UCLASS()
class FLASHPOINT_API UShotgunCrosshair : public UCrosshair
{
	GENERATED_BODY()
	
protected:
	virtual void UpdateCrosshair_Internal() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Left;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Right;
};
