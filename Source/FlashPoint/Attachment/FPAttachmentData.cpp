// Fill out your copyright notice in the Description page of Project Settings.


#include "FPAttachmentData.h"

#include "AttachmentBase.h"
#include "FPGameplayTags.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPAttachmentData)

UFPAttachmentData* UFPAttachmentData::Get(const FGameplayTag& AttachmentTypeTag)
{
	return UFPAssetManager::GetAssetByTag<UFPAttachmentData>(FPGameplayTags::Asset::AttachmentData, AttachmentTypeTag);
}

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult UFPAttachmentData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (!AttachmentClass)
	{
		Context.AddError(FText::FromString(TEXT("AttachmentClass isn't set.")));
		Result = EDataValidationResult::Invalid;
	}
	
	return Result;
}
#endif
