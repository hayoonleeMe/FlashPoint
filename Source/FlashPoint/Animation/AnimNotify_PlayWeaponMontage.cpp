// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_PlayWeaponMontage.h"

#include "GameFramework/Character.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

void UAnimNotify_PlayWeaponMontage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MontageToPlay)
	{
		return;
	}

	if (AWeapon_Base* Weapon = GetEquippedWeapon(MeshComp))
	{
		USkeletalMeshComponent* WeaponMeshComp = Weapon->FindComponentByClass<USkeletalMeshComponent>();
		if (WeaponMeshComp && WeaponMeshComp->GetSkeletalMeshAsset())
		{
			USkeleton* EquippedWeaponSkeleton = WeaponMeshComp->GetSkeletalMeshAsset()->GetSkeleton();
			USkeleton* MontageWeaponSkeleton = MontageToPlay->GetSkeleton();
			if (EquippedWeaponSkeleton && EquippedWeaponSkeleton == MontageWeaponSkeleton)
			{
				// 캐릭터가 장착 중인 무기와 MontageToPlay Anim Montage의 Skeleton이 모두 유효하고 동일해야만 재생
				if (UAnimInstance* WeaponAnimInstance = WeaponMeshComp->GetAnimInstance())
				{
					WeaponAnimInstance->Montage_Play(MontageToPlay, RateScale);
				}
			}
		}
	}
}

AWeapon_Base* UAnimNotify_PlayWeaponMontage::GetEquippedWeapon(const USkeletalMeshComponent* MeshComp)
{
	if (AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr)
	{
		if (UWeaponManageComponent* WeaponManageComp = Owner->FindComponentByClass<UWeaponManageComponent>())
		{
			return WeaponManageComp->GetEquippedWeapon();
		}	
	}
	return nullptr;
}
