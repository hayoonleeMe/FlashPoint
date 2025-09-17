// Fill out your copyright notice in the Description page of Project Settings.


#include "SuccessConfirmedPage.h"

#include "Components/Button.h"

void USuccessConfirmedPage::Input_UI_Confirm()
{
	// OK
	Button_Ok->OnClicked.Broadcast();
}
