// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlayWeaponMontage.generated.h"

class AWeapon_Base;

/**
 * 캐릭터가 장착한 무기 액터의 Skeleton에 대한 Anim Montage를 재생한다.
 */
UCLASS()
class FLASHPOINT_API UAnimNotify_PlayWeaponMontage : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// 캐릭터가 장착한 무기 액터를 반환한다.
	static AWeapon_Base* GetEquippedWeapon(const USkeletalMeshComponent* MeshComp);

protected:
	UPROPERTY(EditAnywhere, Category=AnimNotify)
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY(EditAnywhere, Category=AnimNotify)
	float RateScale = 1.f;
};
