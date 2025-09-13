// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignScreen.generated.h"

class USuccessConfirmedPage;
class UConfirmSignUpPage;
class USignUpPage;
class USignInPage;
class UWidgetSwitcher;

/**
 * Sign In, Sign Up을 수행할 게임 초기 화면
 */
UCLASS()
class FLASHPOINT_API USignScreen : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USignInPage> SignInPage;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USignUpPage> SignUpPage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UConfirmSignUpPage> ConfirmSignUpPage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USuccessConfirmedPage> SuccessConfirmedPage;

	UFUNCTION()
	void ShowSignInPage();
	
	UFUNCTION()
	void ShowSignUpPage();

	void ShowConfirmSignUpPage();

	UFUNCTION()
	void ShowSuccessConfirmedPage();

	UFUNCTION()
	void OnQuitButtonClicked();

	UFUNCTION()
	void OnSignInButtonClicked();

	UFUNCTION()
	void OnSignUpButtonClicked();

	UFUNCTION()
	void OnConfirmButtonClicked();

	void OnSignInSucceeded();

	void OnSignUpSucceeded(const FString& CodeDeliveryDestination);

	void OnConfirmSignUpSucceeded();
};
