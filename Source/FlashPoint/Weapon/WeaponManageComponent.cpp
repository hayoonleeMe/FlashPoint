// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManageComponent.h"

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

	DOREPLIFETIME(UWeaponManageComponent, Slots);
	DOREPLIFETIME(UWeaponManageComponent, ActiveSlotIndex);
	DOREPLIFETIME(UWeaponManageComponent, EquippedWeapon);
}

void UWeaponManageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Slots.AddDefaulted(NumSlots);
}

bool UWeaponManageComponent::HasAuthority() const
{
	return GetOwner() != nullptr && GetOwner()->HasAuthority();
}

void UWeaponManageComponent::ServerEquipWeaponAtSlot_Implementation(int32 SlotNumber)
{
	check(Slots.IsValidIndex(SlotNumber - 1));

	if (ActiveSlotIndex == (SlotNumber - 1))
	{
		// 동일한 슬롯 선택
		return;
	}

	// 현재 무기 장착 중이면 장착 해제
	UnEquipWeapon();

	// 현재 슬롯 업데이트
	ActiveSlotIndex = SlotNumber - 1;

	// 새 슬롯의 무기 장착
	EquipWeaponInternal(Slots[ActiveSlotIndex]);
}

void UWeaponManageComponent::EquipNewWeapon(const TSubclassOf<AWeapon_Base>& WeaponToEquipClass)
{
	if (!HasAuthority() || !WeaponToEquipClass)
	{
		return;
	}

	// 현재 무기 장착 중이면 장착 해제
	UnEquipWeapon();

	// 현재 슬롯의 무기 장착
	EquipWeaponInternal(WeaponToEquipClass);
}

void UWeaponManageComponent::ServerUnEquipWeapon_Implementation()
{
	UnEquipWeapon();
}

void UWeaponManageComponent::EquipWeaponInternal(const TSubclassOf<AWeapon_Base>& WeaponClass)
{
	if (WeaponClass)
	{
		APawn* OwningPawn = GetOwner<APawn>();
		check(OwningPawn);

		Slots[ActiveSlotIndex] = WeaponClass;

		// Spawn
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwningPawn;

		EquippedWeapon = GetWorld()->SpawnActor<AWeapon_Base>(WeaponClass, SpawnParams);

		// Attach
		USceneComponent* AttachTargetComp = OwningPawn->GetRootComponent();
		if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
		{
			AttachTargetComp = Character->GetMesh();
		}

		const FWeaponEquipInfo& EquipInfo = EquippedWeapon->GetEquipInfo();
		EquippedWeapon->SetActorRelativeTransform(EquipInfo.AttachTransform);
		EquippedWeapon->AttachToComponent(AttachTargetComp, FAttachmentTransformRules::KeepRelativeTransform, EquipInfo.AttachSocketName);
		
		EquippedWeapon->OnEquipped();
	}
}

void UWeaponManageComponent::UnEquipWeapon()
{
	if (IsValid(EquippedWeapon))
	{
		// 무기를 장착 해제하면 Destroy하고, 무기의 Destroyed()에서 OnUnEquipped() 호출
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}
}

void UWeaponManageComponent::OnRep_EquippedWeapon()
{
	if (IsValid(EquippedWeapon))
	{
		// 클라이언트에서 무기의 OnEquipped() 호출
		EquippedWeapon->OnEquipped();
	}
}
