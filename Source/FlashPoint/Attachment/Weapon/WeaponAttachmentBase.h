// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttachmentInterface.h"
#include "Attachment/AttachmentBase.h"
#include "WeaponAttachmentBase.generated.h"

/**
 * 무기 부착물 기본 액터 클래스
 */
UCLASS(Abstract)
class FLASHPOINT_API AWeaponAttachmentBase : public AAttachmentBase, public IWeaponAttachmentInterface
{
	GENERATED_BODY()

public:
	AWeaponAttachmentBase();
	
	// Begin IWeaponAttachmentInterface
	virtual void ShowWeaponAttachment(bool bShow) override;
	// End IWeaponAttachmentInterface
};
