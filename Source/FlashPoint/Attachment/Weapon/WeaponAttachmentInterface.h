// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponAttachmentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UWeaponAttachmentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 무기 부착물이 구현할 인터페이스
 */
class FLASHPOINT_API IWeaponAttachmentInterface
{
	GENERATED_BODY()
	
public:
	// 무기 부착물의 표시 여부를 변경한다.
	virtual void ShowWeaponAttachment(bool bShow) { }
	
	// 무기가 장착될 때 호출된다.
	virtual void OnWeaponEquipped() { }
	
	// 무기가 장착 해제될 때 호출된다.
	virtual void OnWeaponUnEquipped() { }
	
	// 무기 부착물의 ADS 시작 로직을 수행한다.
	virtual void StartAimDownSight() { }
	
	// 무기 부착물의 ADS 종료 로직을 수행한다.
	virtual void StopAimDownSight() { }
};
