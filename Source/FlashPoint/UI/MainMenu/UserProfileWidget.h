// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserProfileWidget.generated.h"

class UButton;
class UTextBlock;

/**
 * 접속한 User의 Profile을 표시하는 위젯
 *
 * Sign Out 기능을 포함한다.
 */
UCLASS()
class FLASHPOINT_API UUserProfileWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Username;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Email;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SignOut;

	UFUNCTION()
	void OnSignOutButtonClicked();

	void OnSignOutSucceeded();
};
