// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base.h"

#include "FPLogChannels.h"
#include "Data/FPCosmeticData.h"
#include "GameFramework/Character.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Weapon_Base) 

AWeapon_Base::AWeapon_Base()
{
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	bReplicates = true;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh Component"));
	SetRootComponent(WeaponMeshComponent);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComponent->SetHiddenInGame(true);
}

void AWeapon_Base::Destroyed()
{
	Super::Destroyed();
	
	OnUnEquipped();
}


void AWeapon_Base::OnEquipped()
{
	LinkWeaponAnimLayer(false);
	PlayMontage(EquipInfo.EquipMontage);

	GetWorldTimerManager().SetTimer(ShowWeaponTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		WeaponMeshComponent->SetHiddenInGame(false);
	}), 0.2f, false);
}

void AWeapon_Base::OnUnEquipped()
{
	LinkWeaponAnimLayer(true);
	PlayMontage(EquipInfo.UnEquipMontage);

	WeaponMeshComponent->SetHiddenInGame(true);
}

void AWeapon_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon_Base::LinkWeaponAnimLayer(bool bUseDefault) const
{
	const UFPCosmeticData* CosmeticData = UFPAssetManager::GetAssetById<UFPCosmeticData>(TEXT("CosmeticData"));
	check(CosmeticData);

	if (ACharacter* OwningCharacter = GetOwner<ACharacter>())
	{
		if (USkeletalMeshComponent* OwnerMeshComponent = OwningCharacter->GetMesh())
		{
			TSubclassOf<UAnimInstance> Class = bUseDefault ? CosmeticData->GetDefaultAnimLayer() : CosmeticData->SelectAnimLayer(EquipInfo.CosmeticTags); 
			OwnerMeshComponent->LinkAnimClassLayers(Class);
		}
	}
}

void AWeapon_Base::PlayMontage(UAnimMontage* MontageToPlay) const
{
	if (ACharacter* OwningCharacter = GetOwner<ACharacter>())
	{
		OwningCharacter->PlayAnimMontage(MontageToPlay);
	}
}
