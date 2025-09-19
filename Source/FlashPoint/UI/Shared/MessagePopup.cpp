// Fill out your copyright notice in the Description page of Project Settings.


#include "MessagePopup.h"

#include "Component/UIManageComponent.h"
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MessagePopup)

void UMessagePopup::Input_UI_Back()
{
	// Cancel 수행
	Button_Close->OnClicked.Broadcast();
}

void UMessagePopup::SetTitleText(const FString& Str)
{
	Text_Title->SetText(FText::FromString(Str));
}

void UMessagePopup::SetMessageText(const FString& Str)
{
	Text_Message->SetText(FText::FromString(Str));
}

void UMessagePopup::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Close->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
}

void UMessagePopup::OnCloseButtonClicked()
{
	if (UUIManageComponent* UIManageComponent = UUIManageComponent::Get(GetOwningPlayer()))
	{
		UIManageComponent->RemoveWidget(EWidgetLayer::Popup, this);
	}
}
