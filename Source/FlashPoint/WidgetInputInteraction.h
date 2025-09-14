// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetInputInteraction.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UWidgetInputInteraction : public UInterface
{
	GENERATED_BODY()
};

/**
 * 특정 Input이 발생할 때 Widget에서 수행할 작업을 정의하는 인터페이스
 */
class FLASHPOINT_API IWidgetInputInteraction
{
	GENERATED_BODY()

public:
	virtual void Input_UI_Back() {}
};
