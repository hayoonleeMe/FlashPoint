// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base.h"

#include "FPGameplayTags.h"
#include "FPLogChannels.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Data/FPCosmeticData.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
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

	BulletsPerCartridge = 1;
}

void AWeapon_Base::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon_Base, TagStacks);
}

FVector AWeapon_Base::GetWeaponTargetingSourceLocation() const
{
	return WeaponMeshComponent->GetSocketLocation(TEXT("MuzzleFlash"));
}

void AWeapon_Base::OnEquipped()
{
	LinkWeaponAnimLayer(false);
	PlayOwningCharacterMontage(EquipInfo.EquipMontage);

	GetWorldTimerManager().SetTimer(ShowWeaponTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		WeaponMeshComponent->SetHiddenInGame(false);
	}), 0.2f, false);
}

void AWeapon_Base::OnUnEquipped()
{
	LinkWeaponAnimLayer(true);
	PlayOwningCharacterMontage(EquipInfo.UnEquipMontage);

	WeaponMeshComponent->SetHiddenInGame(true);
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
		TracerComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TracerSystem, WeaponMeshComponent, TEXT("MuzzleFlash"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
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
	Super::BeginPlay();

	InitializeTagStacks();
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

void AWeapon_Base::PlayOwningCharacterMontage(UAnimMontage* MontageToPlay) const
{
	if (ACharacter* OwningCharacter = GetOwner<ACharacter>())
	{
		OwningCharacter->PlayAnimMontage(MontageToPlay);
	}
}

void AWeapon_Base::InitializeTagStacks()
{
	if (HasAuthority())
	{
		TagStacks.AddTagStack(FPGameplayTags::Weapon_Data_Ammo, MagCapacity);
	}
}
