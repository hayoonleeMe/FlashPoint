// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManageComponent.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIManageComponent)

UUIManageComponent::UUIManageComponent()
{
}

void UUIManageComponent::BeginPlay()
{
	Super::BeginPlay();

	if (MainWidgetClass)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
		{
			if (PC->IsLocalController())
			{
				MainWidget = CreateWidget(PC, MainWidgetClass);
				MainWidget->AddToViewport();
			}
		}
	}
}
