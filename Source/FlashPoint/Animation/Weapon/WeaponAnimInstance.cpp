// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAnimInstance.h"

#include "Animation/WeaponLayerAnimInstance.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponAnimInstance)

void UWeaponAnimInstance::OnWeaponEquipped(const AActor* Owner)
{
	// Caching Owner Weapon Layer
	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(Owner))
	{
		if (const USkeletalMeshComponent* SKM = OwnerCharacter->GetMesh())
		{
			const TArray<UAnimInstance*>& Layers = SKM->GetLinkedAnimInstances();
			if (Layers.Num())
			{
				OwnerWeaponLayer = Cast<UWeaponLayerAnimInstance>(Layers[0]);
			}
		}
	}
}

void UWeaponAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (OwnerWeaponLayer)
	{
		OwnerWeaponLayer->RetrieveWeaponReloadData(bIsReloading, WeaponBlockAlpha, bUseWeaponUp);
	}
}
