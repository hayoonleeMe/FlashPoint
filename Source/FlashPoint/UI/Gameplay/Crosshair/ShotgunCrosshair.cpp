// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotgunCrosshair.h"

#include "Components/Image.h"
#include "Components/OverlaySlot.h"

void UShotgunCrosshair::UpdateCrosshair(float AimSpread)
{
	AimSpread *= CrosshairSpreadScale;

	if (UOverlaySlot* LeftSlot = Cast<UOverlaySlot>(Image_Left->Slot))
	{
		LeftSlot->SetPadding({ -AimSpread, 0.f, 0.f, 0.f });
	}
	if (UOverlaySlot* RightSlot = Cast<UOverlaySlot>(Image_Right->Slot))
	{
		RightSlot->SetPadding({ 0.f, 0.f, -AimSpread, 0.f });
	}
}
