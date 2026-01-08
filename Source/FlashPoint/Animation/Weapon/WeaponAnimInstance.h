// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

class UWeaponLayerAnimInstance;

/**
 * 무기의 Skeletal Mesh에서 사용할 Base Anim Instance
 */
UCLASS()
class FLASHPOINT_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void OnWeaponEquipped(const AActor* Owner);
	
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	TObjectPtr<UWeaponLayerAnimInstance> OwnerWeaponLayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reload")
	bool bIsReloading;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reload")
	float WeaponBlockAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reload")
	bool bUseWeaponUp;
};
