// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attachment/AttachmentManageComponent.h"
#include "WeaponAttachmentComponent.generated.h"

class IWeaponAttachmentInterface;

/**
 * 무기 전용 부착물을 전담해 관리하는 컴포넌트
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UWeaponAttachmentComponent : public UAttachmentManageComponent
{
	GENERATED_BODY()

public:
	const TArray<TScriptInterface<IWeaponAttachmentInterface>>& GetWeaponAttachmentInterfaces() const { return WeaponAttachmentInterfaces; }
	
	// 현재 장착 중인 UpperRail 부착물의 스탯 객체를 반환한다.
	// UpperRail을 장착 중이 아니면 nullptr를 반환한다.
	UFUNCTION(BlueprintCallable)
	const UAttachmentStat_UpperRail* GetUpperRailStat() const;

protected:
	// Begin UAttachmentManageComponent
	virtual void OnAttachmentAdded(EAttachmentSlot AttachmentSlot, const FEquippedAttachment& EquippedAttachment) override;
	virtual void OnAttachmentRemoved(EAttachmentSlot AttachmentSlot, const FEquippedAttachment& EquippedAttachment) override;
	// End UAttachmentManageComponent
	
private:
	// 현재 장착 중인 부착물 액터를 IWeaponAttachmentInterface로 변환해 저장한 배열
	UPROPERTY()
	TArray<TScriptInterface<IWeaponAttachmentInterface>> WeaponAttachmentInterfaces;
};
