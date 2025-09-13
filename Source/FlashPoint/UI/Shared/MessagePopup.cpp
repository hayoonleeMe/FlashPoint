// Fill out your copyright notice in the Description page of Project Settings.


#include "MessagePopup.h"

#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MessagePopup)

void UMessagePopup::SetTitleText(const FString& Str)
{
	Text_Title->SetText(FText::FromString(Str));
}

void UMessagePopup::SetMessageText(const FString& Str)
{
	Text_Message->SetText(FText::FromString(Str));
}
