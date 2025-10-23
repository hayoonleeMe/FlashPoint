// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManageComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "Data/FPAbilitySystemData.h"
#include "Data/FPRecoilData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/BasePlayerController.h"
#include "System/FPAssetManager.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponManageComponent) 

UWeaponManageComponent* UWeaponManageComponent::Get(const AActor* OwnerActor)
{
	if (IsValid(OwnerActor))
	{
		return OwnerActor->FindComponentByClass<UWeaponManageComponent>();
	}
	return nullptr;
}

UWeaponManageComponent::UWeaponManageComponent()
{
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	NumSlots = 3;
	ActiveSlotIndex = 0;

	AimSpreadMovementInterpSpeed = 4.f;
	AimSpreadMovingOffset = 15.f;
	AimSpreadFallingOffset = 20.f;
	AimSpreadCrouchingOffset = -10.f;
	AimSpreadSprintingOffset = 25.f;
}

void UWeaponManageComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponManageComponent, ActiveSlotIndex);
	DOREPLIFETIME(UWeaponManageComponent, EquippedWeapon);
	DOREPLIFETIME(UWeaponManageComponent, WeaponEquipStateUpdateCounter);
	DOREPLIFETIME(UWeaponManageComponent, AmmoTagStacks);
}

void UWeaponManageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (HasAuthority())
	{
		WeaponSlots.AddDefaulted(NumSlots);
	}
}

int32 UWeaponManageComponent::GetReserveAmmoStackCount(const AActor* Actor)
{
	if (IsValid(Actor))
	{
		if (UWeaponManageComponent* WeaponManageComponent = Actor->FindComponentByClass<UWeaponManageComponent>())
		{
			if (const AWeapon_Base* Weapon = WeaponManageComponent->GetEquippedWeapon())
			{
				return WeaponManageComponent->GetAmmoTagStacks().GetStackCount(Weapon->GetWeaponTypeTag());
			}
		}
	}
	return 0;
}

void UWeaponManageComponent::RegisterAmmoTagStackChangedEvent(const FGameplayTag& Tag, const FOnAmmoTagStackChangedDelegate::FDelegate& InDelegate)
{
	AmmoTagStackChangedEventMap.FindOrAdd(Tag).Add(InDelegate);
}

void UWeaponManageComponent::RegisterAllReserveAmmoChangedEvent(const FOnAmmoTagStackChangedDelegate::FDelegate& InDelegate)
{
	for (const auto& Pair : InitialReserveAmmoMap)
	{
		AmmoTagStackChangedEventMap.FindOrAdd(Pair.Key).Add(InDelegate);
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

	// Equip State 변경을 알림
	++WeaponEquipStateUpdateCounter;
	OnWeaponEquipStateChangedDelegate.Broadcast(ActiveSlotIndex, EquippedWeapon);
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

	// Equip State 변경을 알림
	++WeaponEquipStateUpdateCounter;
	OnWeaponEquipStateChangedDelegate.Broadcast(ActiveSlotIndex, EquippedWeapon);
}

void UWeaponManageComponent::ServerUnEquipWeapon_Implementation()
{
	UnEquipWeapon(true);

	// Equip State 변경을 알림
	++WeaponEquipStateUpdateCounter;
	OnWeaponEquipStateChangedDelegate.Broadcast(ActiveSlotIndex, EquippedWeapon);
}

void UWeaponManageComponent::BeginPlay()
{
	Super::BeginPlay();

	// caching (only local)
	ACharacter* Character = GetOwner<ACharacter>();
	if (Character && Character->IsLocallyControlled())
	{
		OwnerCharacter = Character;
		OwnerCharacterMoveComponent = OwnerCharacter->GetCharacterMovement();
		if (ABasePlayerController* BasePC = Character->GetController<ABasePlayerController>())
		{
			BasePC->OnPlayerStateReplicatedDelegate.AddLambda([this](APlayerState* PS)
			{
				OwnerAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PS);
			});
		}
	}

	// Bind Callback
	AmmoTagStacks.OnTagStackChangedDelegate.AddUObject(this, &ThisClass::OnAmmoTagStackChanged);
		
	if (HasAuthority())
	{
		// Initialize Ammo
		AmmoTagStacks.AddTagStack(FPGameplayTags::Weapon::Data::Ammo, 0);
		for (const auto& Pair : InitialReserveAmmoMap)
		{
			AmmoTagStacks.AddTagStack(Pair.Key, Pair.Value);
		}
	}
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
		UFPAbilitySystemData::GiveDataToAbilitySystem(OwningPawn, EquippedWeapon->GetWeaponTypeTag());

		// 장착한 무기의 탄창에 있는 총알 수로 업데이트
		AmmoTagStacks.AddTagStack(FPGameplayTags::Weapon::Data::Ammo, EquippedWeapon->GetMagCapacity());

		// Reserve Ammo를 HUD에 업데이트하기 위해 동일한 값으로 설정
		AmmoTagStacks.AddTagStack(EquippedWeapon->GetWeaponTypeTag(), AmmoTagStacks.GetStackCount(EquippedWeapon->GetWeaponTypeTag()));
		
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
		UFPAbilitySystemData::GiveDataToAbilitySystem(OwningPawn, EquippedWeapon->GetWeaponTypeTag());

		// 장착한 무기의 탄창에 있는 총알 수로 업데이트
		AmmoTagStacks.AddTagStack(FPGameplayTags::Weapon::Data::Ammo, EquippedWeapon->GetServerRemainAmmo());

		// Reserve Ammo를 HUD에 업데이트하기 위해 동일한 값으로 설정
		AmmoTagStacks.AddTagStack(EquippedWeapon->GetWeaponTypeTag(), AmmoTagStacks.GetStackCount(EquippedWeapon->GetWeaponTypeTag()));
		
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
		UFPAbilitySystemData::RemoveDataFromAbilitySystem(GetOwner(), EquippedWeapon->GetWeaponTypeTag());

		if (bDestroy)
		{
			// 클라이언트에서 장착 해제한 무기 액터의 OnUnEquipped() 호출을 보장하기 위해 일정 시간 뒤에 Destroy
			EquippedWeapon->SetLifeSpan(5.f);
			
			// 슬롯에서 제거
			WeaponSlots[ActiveSlotIndex] = nullptr;
		}

		// 탄창에 남은 총알 수 저장
		EquippedWeapon->SetServerRemainAmmo(AmmoTagStacks.GetStackCount(FPGameplayTags::Weapon::Data::Ammo));

		// 빈 탄창으로 업데이트
		AmmoTagStacks.AddTagStack(FPGameplayTags::Weapon::Data::Ammo, 0);

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
	
	// 장착 무기 변경 시 반동 데이터 초기화
	ClearRecoil();

	// UnEquip에 따른 WeaponOffset 초기화
	AimSpreadWeaponOffset = 0.f;
	
	if (IsValid(EquippedWeapon))
	{
		// 클라이언트에서 장착된 무기 처리
		EquippedWeapon->OnEquipped();

		// Equip에 따라 RecoilData 캐싱 및 WeaponOffset 설정
		CurrentRecoilData = UFPAssetManager::GetAssetByTag<UFPRecoilData>(UFPRecoilData::GetRecoilDataTagByWeaponType(EquippedWeapon->GetWeaponTypeTag()));
		if (CurrentRecoilData)
		{
			AimSpreadWeaponOffset = CurrentRecoilData->AimSpreadWeaponOffset;
		}
		else
		{
			UE_LOG(LogFP, Error, TEXT("[%hs] No RecoilData found for equipped weapon."), __FUNCTION__);
		}
	}

	OnEquippedWeaponChanged.Broadcast(EquippedWeapon);
	UpdateCurrentAimSpread();
}

void UWeaponManageComponent::OnRep_WeaponEquipStateUpdateCounter()
{
	OnWeaponEquipStateChangedDelegate.Broadcast(ActiveSlotIndex, EquippedWeapon);
}

void UWeaponManageComponent::OnAmmoTagStackChanged(const FGameplayTag& Tag, int32 StackCount)
{
	// 변경된 Tag에 대해 등록된 델레게이트가 있다면 브로드캐스트
	if (FOnAmmoTagStackChangedDelegate* Delegate = AmmoTagStackChangedEventMap.Find(Tag))
	{
		Delegate->Broadcast(Tag, StackCount);
	}
}

void UWeaponManageComponent::ApplyRecoil()
{
	if (!CurrentRecoilData || !CurrentRecoilData->VerticalRecoilCurve || !CurrentRecoilData->HorizontalRecoilCurve)
	{
		return;
	}

	// ShotCount 누적
	++RecoilShotCount;

	const float RecoilPitch = CurrentRecoilData->VerticalRecoilCurve->GetFloatValue(RecoilShotCount);
	// 좌우 반동은 무작위 방향 적용
	const float RecoilYaw = CurrentRecoilData->HorizontalRecoilCurve->GetFloatValue(RecoilShotCount) * (FMath::RandBool() ? 1.f : -1.f);
	TargetRecoilOffset += FVector2D(RecoilPitch, RecoilYaw);
	bShouldApplyRecoil = true;

	AimSpreadRecoilOffset = FMath::Min(AimSpreadRecoilOffset + CurrentRecoilData->AimSpreadIncrease, CurrentRecoilData->MaxAimSpread);
	UpdateCurrentAimSpread();
	bShouldApplyRecovery = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RecoveryTimer, FTimerDelegate::CreateUObject(this, &ThisClass::StartRecovery), CurrentRecoilData->RecoveryDelay, false);;
	}
}

void UWeaponManageComponent::ClearRecoil()
{
	RecoilShotCount = 0;
	CurrentRecoilOffset = TargetRecoilOffset = FVector2D(0.f);
	bShouldApplyRecoil = bShouldApplyRecovery = false;
	CurrentRecoilData = nullptr;

	AimSpreadRecoilOffset = 0.f;
	UpdateCurrentAimSpread();
}

void UWeaponManageComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CurrentRecoilData)
	{
		return;
	}
	
	if (bShouldApplyRecoil && OwnerCharacter)
	{
		FVector2D NewRecoilOffset = FMath::Vector2DInterpTo(CurrentRecoilOffset, TargetRecoilOffset, DeltaTime, CurrentRecoilData->RecoilInterpSpeed);
		const FVector2D DeltaRecoilOffset = NewRecoilOffset - CurrentRecoilOffset;
		CurrentRecoilOffset = NewRecoilOffset;
	
		if (!DeltaRecoilOffset.IsNearlyZero())
		{
			OwnerCharacter->AddControllerPitchInput(-DeltaRecoilOffset.X);
			OwnerCharacter->AddControllerYawInput(DeltaRecoilOffset.Y);
		}
		else
		{
			bShouldApplyRecoil = false;
		}
	}

	if (bShouldApplyRecovery)
	{
		AimSpreadRecoilOffset = FMath::FInterpTo(AimSpreadRecoilOffset, 0.f, DeltaTime, CurrentRecoilData->AimSpreadRecoverySpeed);
		if (FMath::IsNearlyZero(CurrentAimSpread))
		{
			AimSpreadRecoilOffset = 0.f;
			bShouldApplyRecovery = false;
		}
	}

	AimSpreadMovementOffset = FMath::FInterpTo(AimSpreadMovementOffset, CalculateAimSpreadMovementOffset(), DeltaTime, AimSpreadMovementInterpSpeed);
	UpdateCurrentAimSpread();
}

void UWeaponManageComponent::StartRecovery()
{
	RecoilShotCount = 0;
	CurrentRecoilOffset = TargetRecoilOffset = FVector2D(0.f);
	bShouldApplyRecovery = true;
}

float UWeaponManageComponent::CalculateAimSpreadMovementOffset() const
{
	float MovementOffset = 0.f;
	if (OwnerCharacter && OwnerCharacterMoveComponent)
	{
		// Moving
		if (OwnerCharacterMoveComponent->Velocity.SizeSquared2D() > KINDA_SMALL_NUMBER)
		{
			MovementOffset += AimSpreadMovingOffset;
		}

		// In Air
		if (OwnerCharacterMoveComponent->IsFalling())
		{
			MovementOffset += AimSpreadFallingOffset;
		}

		// Crouching
		if (OwnerCharacter->bIsCrouched)
		{
			MovementOffset += AimSpreadCrouchingOffset;
		}

		// Sprinting
		if (OwnerAbilitySystemComponent && OwnerAbilitySystemComponent->HasMatchingGameplayTag(FPGameplayTags::CharacterState::IsSprinting))
		{
			MovementOffset += AimSpreadSprintingOffset;
		}
	}
	return MovementOffset;
}

void UWeaponManageComponent::UpdateCurrentAimSpread()
{
	CurrentAimSpread = AimSpreadWeaponOffset + AimSpreadRecoilOffset + AimSpreadMovementOffset;
	OnAimSpreadChangedDelegate.Broadcast(CurrentAimSpread);
}
