// Fill out your copyright notice in the Description page of Project Settings.


#include "APIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(APIData)

FString UAPIData::GetAPIEndpoint(const FGameplayTag& APIEndpoint)
{
	const FString ResourceName = Resources.FindChecked(APIEndpoint);
	return InvokeURL + "/" + Stage + "/" + ResourceName;
}
