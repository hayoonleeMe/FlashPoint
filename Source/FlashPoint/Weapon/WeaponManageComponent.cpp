﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManageComponent.h"

#include "Character/FPCharacter.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponManageComponent) 

UWeaponManageComponent::UWeaponManageComponent()
{
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;

	NumSlots = 3;
	ActiveSlotIndex = 0;
}

void UWeaponManageComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponManageComponent, ActiveSlotIndex);
	DOREPLIFETIME(UWeaponManageComponent, EquippedWeapon);
}

void UWeaponManageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (HasAuthority())
	{
		WeaponSlots.AddDefaulted(NumSlots);
	}
}

bool UWeaponManageComponent::HasAuthority() const
{
	return GetOwner() != nullptr && GetOwner()->HasAuthority();
}

void UWeaponManageComponent::ServerEquipWeaponAtSlot_Implementation(int32 SlotNumber)
{
	check(WeaponSlots.IsValidIndex(SlotNumber - 1));

	if (ActiveSlotIndex == (SlotNumber - 1))
	{
		// 동일한 슬롯 선택
		return;
	}

	// 현재 무기 장착 중이면 장착 해제
	UnEquipWeapon(false);

	// 현재 슬롯 업데이트
	ActiveSlotIndex = SlotNumber - 1;

	// 새 슬롯의 무기 장착
	EquipWeaponInternal(WeaponSlots[ActiveSlotIndex]);
}

void UWeaponManageComponent::EquipNewWeapon(const TSubclassOf<AWeapon_Base>& WeaponToEquipClass)
{
	if (!HasAuthority() || !WeaponToEquipClass)
	{
		return;
	}

	// 현재 무기 장착 중이면 장착 해제하고 Destroy
	UnEquipWeapon(true);

	// 현재 슬롯의 무기 장착
	EquipWeaponInternal(WeaponToEquipClass);
}

void UWeaponManageComponent::ServerUnEquipWeapon_Implementation()
{
	UnEquipWeapon(false);
}

void UWeaponManageComponent::EquipWeaponInternal(const TSubclassOf<AWeapon_Base>& WeaponClass)
{
	if (WeaponClass)
	{
		APawn* OwningPawn = GetOwner<APawn>();
		check(OwningPawn);

		// Spawn
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwningPawn;

		EquippedWeapon = GetWorld()->SpawnActor<AWeapon_Base>(WeaponClass, SpawnParams);
		WeaponSlots[ActiveSlotIndex] = EquippedWeapon;

		// Attach
		USceneComponent* AttachTargetComp = OwningPawn->GetRootComponent();
		if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
		{
			AttachTargetComp = Character->GetMesh();
		}

		const FWeaponEquipInfo& EquipInfo = EquippedWeapon->GetEquipInfo();
		EquippedWeapon->SetActorRelativeTransform(EquipInfo.AttachTransform);
		EquippedWeapon->AttachToComponent(AttachTargetComp, FAttachmentTransformRules::KeepRelativeTransform, EquipInfo.AttachSocketName);

		// Give Data to Owner ASC
		if (AFPCharacter* OwningCharacter = Cast<AFPCharacter>(OwningPawn))
		{
			OwningCharacter->ApplyAbilitySystemData(EquippedWeapon->GetWeaponTypeTag());
		}
		
		EquippedWeapon->OnEquipped();

		OnEquippedWeaponChanged.Broadcast(EquippedWeapon);
	}
}

void UWeaponManageComponent::EquipWeaponInternal(AWeapon_Base* WeaponInSlot)
{
	if (IsValid(WeaponInSlot))
	{
		APawn* OwningPawn = GetOwner<APawn>();
		check(OwningPawn);

		EquippedWeapon = WeaponInSlot;

		// Attach
		USceneComponent* AttachTargetComp = OwningPawn->GetRootComponent();
		if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
		{
			AttachTargetComp = Character->GetMesh();
		}

		const FWeaponEquipInfo& EquipInfo = EquippedWeapon->GetEquipInfo();
		EquippedWeapon->SetActorRelativeTransform(EquipInfo.AttachTransform);
		EquippedWeapon->AttachToComponent(AttachTargetComp, FAttachmentTransformRules::KeepRelativeTransform, EquipInfo.AttachSocketName);

		// Give Data to Owner ASC
		if (AFPCharacter* OwningCharacter = Cast<AFPCharacter>(OwningPawn))
		{
			OwningCharacter->ApplyAbilitySystemData(EquippedWeapon->GetWeaponTypeTag());
		}
		
		EquippedWeapon->OnEquipped();

		OnEquippedWeaponChanged.Broadcast(EquippedWeapon);
	}
}

void UWeaponManageComponent::UnEquipWeapon(bool bDestroy)
{
	if (IsValid(EquippedWeapon))
	{
		// Detach
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		
		// Remove Data from Owner ASC
		if (AFPCharacter* OwningCharacter = GetOwner<AFPCharacter>())
		{
			OwningCharacter->RemoveAbilitySystemData(EquippedWeapon->GetWeaponTypeTag());
		}

		if (bDestroy)
		{
			// 클라이언트에서 장착 해제한 무기 액터의 OnUnEquipped() 호출을 보장하기 위해 일정 시간 뒤에 Destroy
			EquippedWeapon->SetLifeSpan(5.f);
		}

		EquippedWeapon->OnUnEquipped();
		EquippedWeapon = nullptr;

		OnEquippedWeaponChanged.Broadcast(EquippedWeapon);
	}
}

void UWeaponManageComponent::OnRep_EquippedWeapon(AWeapon_Base* UnEquippedWeapon)
{
	if (IsValid(UnEquippedWeapon))
	{
		// 클라이언트에서 장착 해제된 무기 처리
		UnEquippedWeapon->OnUnEquipped();
	}
	
	if (IsValid(EquippedWeapon))
	{
		// 클라이언트에서 장착된 무기 처리
		EquippedWeapon->OnEquipped();
	}

	OnEquippedWeaponChanged.Broadcast(EquippedWeapon);
}
