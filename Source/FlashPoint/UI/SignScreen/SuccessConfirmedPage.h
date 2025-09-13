// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SuccessConfirmedPage.generated.h"

class UButton;

/**
 * Sign Up이 성공했음을 알리는 위젯
 */
UCLASS()
class FLASHPOINT_API USuccessConfirmedPage : public UUserWidget
{
	GENERATED_BODY()

	friend class USignScreen;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Ok;
};
