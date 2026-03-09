// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAttachmentComponent.h"

#include "WeaponAttachmentInterface.h"
#include "Attachment/AttachmentBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponAttachmentComponent)

const UAttachmentStat_UpperRail* UWeaponAttachmentComponent::GetUpperRailStat() const
{
	return GetAttachmentStat<UAttachmentStat_UpperRail>(EAttachmentSlot::UpperRail);
}

void UWeaponAttachmentComponent::OnAttachmentAdded(EAttachmentSlot AttachmentSlot, const FEquippedAttachment& EquippedAttachment)
{
	// 추가된 부착물 액터를 변환해 배열에 저장
	if (TScriptInterface<IWeaponAttachmentInterface> Interface = EquippedAttachment.AttachmentActor)
	{
		WeaponAttachmentInterfaces.Add(Interface);
	}
	
	Super::OnAttachmentAdded(AttachmentSlot, EquippedAttachment);
}

void UWeaponAttachmentComponent::OnAttachmentRemoved(EAttachmentSlot AttachmentSlot, const FEquippedAttachment& EquippedAttachment)
{
	// 제거된 부착물 액터를 배열에서 제거
	if (TScriptInterface<IWeaponAttachmentInterface> Interface = EquippedAttachment.AttachmentActor)
	{
		WeaponAttachmentInterfaces.RemoveSingleSwap(Interface, EAllowShrinking::No);
	}
	
	Super::OnAttachmentRemoved(AttachmentSlot, EquippedAttachment);
}
