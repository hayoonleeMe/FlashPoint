// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultCrosshair.h"

#include "Components/Image.h"
#include "Components/OverlaySlot.h"

void UDefaultCrosshair::UpdateCrosshair(float AimSpread)
{
	AimSpread *= CrosshairSpreadScale;
	
	if (UOverlaySlot* TopSlot = Cast<UOverlaySlot>(Image_Top->Slot))
	{
		TopSlot->SetPadding({ 0.f, -AimSpread, 0.f, 0.f });
	}
	if (UOverlaySlot* BottomSlot = Cast<UOverlaySlot>(Image_Bottom->Slot))
	{
		BottomSlot->SetPadding({ 0.f, 0.f, 0.f, -AimSpread });
	}
	if (UOverlaySlot* LeftSlot = Cast<UOverlaySlot>(Image_Left->Slot))
	{
		LeftSlot->SetPadding({ -AimSpread, 0.f, 0.f, 0.f });
	}
	if (UOverlaySlot* RightSlot = Cast<UOverlaySlot>(Image_Right->Slot))
	{
		RightSlot->SetPadding({ 0.f, 0.f, -AimSpread, 0.f });
	}
}
