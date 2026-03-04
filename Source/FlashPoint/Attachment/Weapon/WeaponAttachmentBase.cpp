// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAttachmentBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponAttachmentBase)

AWeaponAttachmentBase::AWeaponAttachmentBase()
{
	// 무기 부착물은 무기와 같이 초기에 숨겨져야 함
	PrimaryMesh->SetHiddenInGame(true);
}
