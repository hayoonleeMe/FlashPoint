// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "FPWeaponConfigData.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "System/FPAssetManager.h"
#include "Attachment/FPAttachmentData.h"
#include "Attachment/Weapon/WeaponAttachmentComponent.h"
#include "Attachment/Weapon/WeaponAttachmentInterface.h"
#include "Component/DynamicMaterial/CustomFovComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Weapon_Base) 

AWeapon_Base::AWeapon_Base()
{
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	bReplicates = true;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh Component"));
	SetRootComponent(WeaponMeshComponent);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComponent->SetHiddenInGame(true, true);
	
	WeaponAttachmentComponent = CreateDefaultSubobject<UWeaponAttachmentComponent>(TEXT("Weapon Attachment Component"));
}

UMeshComponent* AWeapon_Base::GetAttachmentOwnerMeshComponent() const
{
	return WeaponMeshComponent;
}

void AWeapon_Base::Destroyed()
{
	// 클라에선 장착 중인 상태였다면 제거되기 전에 장착해제 로직 수행
	if (!HasAuthority() && GetOwner())
	{
		OnUnEquipped();
	}
	
	Super::Destroyed();
}

FVector AWeapon_Base::GetWeaponTargetingSourceLocation() const
{
	return WeaponMeshComponent->GetSocketLocation(TEXT("MuzzleFlash"));
}

FTransform AWeapon_Base::GetLeftHandAttachTransform() const
{
	return WeaponMeshComponent->GetSocketTransform(WeaponConfigData->LeftHandAttachSocketName);
}

void AWeapon_Base::OnEquipped()
{
	GetWorldTimerManager().SetTimer(ShowWeaponTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		ShowWeapon(true);
	}), 0.2f, false);
	
	for (const TScriptInterface<IWeaponAttachmentInterface>& Interface : WeaponAttachmentComponent->GetWeaponAttachmentInterfaces())
	{
		if (Interface)
		{
			Interface->OnWeaponEquipped();
		}
	}
}

void AWeapon_Base::OnUnEquipped()
{
	if (UFPAbilitySystemComponent* ASC = GetOwnerASC<UFPAbilitySystemComponent>())
	{
		// 무기 발사 입력 Flush
		ASC->FlushPressedInput(FPGameplayTags::Input::Gameplay::WeaponFire);
	}

	ShowWeapon(false);
	
	for (const TScriptInterface<IWeaponAttachmentInterface>& Interface : WeaponAttachmentComponent->GetWeaponAttachmentInterfaces())
	{
		if (Interface)
		{
			Interface->OnWeaponUnEquipped();
		}
	}
}

void AWeapon_Base::BroadcastWeaponFireEffects_Implementation(const TArray<FVector_NetQuantize>& ImpactPoints, const TArray<FVector_NetQuantize>& EndPoints)
{
	if (APawn* OwningPawn = GetOwner<APawn>())
	{
		if (OwningPawn->GetLocalRole() == ROLE_SimulatedProxy)
		{
			// 클라이언트의 로컬 플레이어를 제외한 다른 플레이어 캐릭터의 Effect 표시
			TriggerWeaponFireEffects(ImpactPoints, EndPoints);
		}
	}
}

void AWeapon_Base::TriggerWeaponFireEffects(const TArray<FVector_NetQuantize>& ImpactPoints, const TArray<FVector_NetQuantize>& EndPoints)
{
	if (!TracerComponent || !TracerComponent->IsActive())
	{
		TracerComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(WeaponConfigData->TracerSystem, WeaponMeshComponent, TEXT("MuzzleFlash"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
		bTracerTrigger = false;
	}

	// Show Tracer
	bTracerTrigger = !bTracerTrigger;
	TracerComponent->SetBoolParameter(TEXT("User.Trigger"), bTracerTrigger);

	// Set Impact Positions
	TArray<FVector> ImpactPositions;
	ImpactPositions.Append(ImpactPoints);
	ImpactPositions.Append(EndPoints);
	
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TracerComponent, TEXT("User.ImpactPositions"), ImpactPositions);

	// TODO : Impact Effect
	for (const FVector& ImpactPoint : ImpactPoints)
	{
		DrawDebugSphere(GetWorld(), ImpactPoint, 6.f, 6, FColor::Red, false, 5.f, 1);
	}
}

void AWeapon_Base::BeginPlay()
{
	// 로컬 클라이언트에서 CustomFovComponent 등록
	if (IsOwnerLocallyControlled() && !CustomFovComponent)
	{
		CustomFovComponent = UCustomFovComponent::CreateComponent(this, TEXT("Custom FOV Component"), GetOwnerASC(), { WeaponMeshComponent });
	}
	
	WeaponAttachmentComponent->OnAttachmentAddedDelegate.AddUObject(this, &ThisClass::OnAttachmentAdded);
	WeaponAttachmentComponent->OnAttachmentRemovedDelegate.AddUObject(this, &ThisClass::OnAttachmentRemoved);
	
	Super::BeginPlay();
	
	WeaponConfigData = UFPAssetManager::GetAssetByTag<UFPWeaponConfigData>(FPGameplayTags::Asset::WeaponConfigData, WeaponTypeTag);
	if (!ensure(WeaponConfigData))
	{
		UE_LOG(LogFP, Warning, TEXT("[%hs] Can't Get WeaponConfigData Asset."), __FUNCTION__);
	}
}

void AWeapon_Base::GetFirstPersonRightHandOffset(FVector& OutLoc, FRotator& OutRot) const
{
	OutLoc = WeaponConfigData->FirstPersonRightHandLocOffset;
	OutRot = WeaponConfigData->FirstPersonRightHandRotOffset;
}

FTransform AWeapon_Base::GetAimDownSightSocketTransform() const
{
	FTransform AttachmentTransform;
	if (WeaponAttachmentComponent->GetAttachmentSocketTransform(AttachmentTransform, EAttachmentSlot::UpperRail, WeaponConfigData->AimDownSightSocketName))
	{
		return AttachmentTransform;
	}
	return WeaponMeshComponent->GetSocketTransform(WeaponConfigData->AimDownSightSocketName);
}

float AWeapon_Base::GetAimDownSightFOV() const
{
	// 스코프를 장착중이면 스코프 스탯 반환
	if (const UAttachmentStat_UpperRail* UpperRailStat = WeaponAttachmentComponent->GetUpperRailStat())
	{
		return UpperRailStat->AimDownSightFOV;
	}
	return WeaponConfigData->AimDownSightFOV;
}

float AWeapon_Base::GetAimDownSightWeaponFOV() const
{
	// 스코프를 장착중이면 스코프 스탯 반환
	if (const UAttachmentStat_UpperRail* UpperRailStat = WeaponAttachmentComponent->GetUpperRailStat())
	{
		return UpperRailStat->AimDownSightWeaponFOV;
	}
	return WeaponConfigData->AimDownSightWeaponFOV;
}

float AWeapon_Base::GetAimDownSightSpeedModifier() const
{
	// 스코프를 장착중이면 스코프 스탯 반환
	if (const UAttachmentStat_UpperRail* UpperRailStat = WeaponAttachmentComponent->GetUpperRailStat())
	{
		return UpperRailStat->AimDownSightSpeedModifier;
	}
	return 0.f;
}

float AWeapon_Base::GetTimeToADS() const
{
	// Speed Modifier를 적용한 TimeToADS를 반환한다.
	return WeaponConfigData->TimeToADS / FMath::Max(1.f + GetAimDownSightSpeedModifier(), 0.1f);
}
}

void AWeapon_Base::StartAimDownSight()
{
	for (const TScriptInterface<IWeaponAttachmentInterface>& Interface : WeaponAttachmentComponent->GetWeaponAttachmentInterfaces())
	{
		if (Interface)
		{
			Interface->StartAimDownSight();
		}
	}
	
	if (CustomFovComponent)
	{
		CustomFovComponent->SetCustomFOV(GetAimDownSightWeaponFOV());
	}
}

void AWeapon_Base::StopAimDownSight()
{
	for (const TScriptInterface<IWeaponAttachmentInterface>& Interface : WeaponAttachmentComponent->GetWeaponAttachmentInterfaces())
	{
		if (Interface)
		{
			Interface->StopAimDownSight();		
		}
	}
	
	if (CustomFovComponent)
	{
		CustomFovComponent->SetCustomFOV(WeaponConfigData->FirstPersonDefaultWeaponFOV);
	}
}

float AWeapon_Base::GetDamageByDistance(float Distance) const
{
	if (WeaponConfigData->DamageFallOffCurve)
	{
		return WeaponConfigData->BaseDamage * WeaponConfigData->DamageFallOffCurve->GetFloatValue(Distance);
	}
	return WeaponConfigData->BaseDamage;
}

UAbilitySystemComponent* AWeapon_Base::GetOwnerASC() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

bool AWeapon_Base::IsOwnerLocallyControlled() const
{
	APawn* OwnerPawn = GetOwner<APawn>();
	return OwnerPawn && OwnerPawn->IsLocallyControlled();
}

void AWeapon_Base::ShowWeapon(bool bShow)
{
	WeaponMeshComponent->SetHiddenInGame(!bShow);

	for (const TScriptInterface<IWeaponAttachmentInterface>& Interface : WeaponAttachmentComponent->GetWeaponAttachmentInterfaces())
	{
		if (Interface)
		{
			Interface->ShowWeaponAttachment(bShow);
		}
	}
}

void AWeapon_Base::OnAttachmentAdded(EAttachmentSlot AttachmentSlot, AActor* AttachmentActor)
{
	if (!IsValid(AttachmentActor))
	{
		return;
	}
	
	if (CustomFovComponent)
	{
		// 추가된 부착물 메시 등록
		AttachmentActor->ForEachComponent<UMeshComponent>(false, [&](UMeshComponent* Component)
		{
			CustomFovComponent->RegisterMesh(Component);
		});
	}

	// Visibility 동기화
	if (IWeaponAttachmentInterface* Interface = Cast<IWeaponAttachmentInterface>(AttachmentActor))
	{
		const bool bShow = !WeaponMeshComponent->bHiddenInGame;
		Interface->ShowWeaponAttachment(bShow);
	}
}

void AWeapon_Base::OnAttachmentRemoved(EAttachmentSlot AttachmentSlot, AActor* AttachmentActor)
{
	if (!IsValid(AttachmentActor))
	{
		return;
	}
	
	if (CustomFovComponent)
	{
		// 제거된 부착물 메시 등록 해제
		AttachmentActor->ForEachComponent<UMeshComponent>(false, [&](UMeshComponent* Component)
		{
			CustomFovComponent->UnRegisterMesh(Component);
		});
	}
	
	// Visibility 동기화
	if (IWeaponAttachmentInterface* Interface = Cast<IWeaponAttachmentInterface>(AttachmentActor))
	{
		const bool bShow = !WeaponMeshComponent->bHiddenInGame;
		Interface->ShowWeaponAttachment(bShow);
	}
}
