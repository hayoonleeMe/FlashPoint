// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Base.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "FPGameplayTags.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/FPAbilitySystemComponent.h"

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
	HeadShotMultiplier = 1.f;

	LeftHandAttachSocketName = TEXT("LeftHandSocket");
}

FVector AWeapon_Base::GetWeaponTargetingSourceLocation() const
{
	return WeaponMeshComponent->GetSocketLocation(TEXT("MuzzleFlash"));
}

FTransform AWeapon_Base::GetLeftHandAttachTransform() const
{
	return WeaponMeshComponent->GetSocketTransform(LeftHandAttachSocketName);
}

void AWeapon_Base::OnEquipped()
{
	GetWorldTimerManager().SetTimer(ShowWeaponTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		WeaponMeshComponent->SetHiddenInGame(false);
	}), 0.2f, false);
}

void AWeapon_Base::OnUnEquipped()
{
	if (UFPAbilitySystemComponent* ASC = GetOwnerASC<UFPAbilitySystemComponent>())
	{
		// 무기 발사 입력 Flush
		ASC->FlushPressedInput(FPGameplayTags::Input::Gameplay::WeaponFire);
	}

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

float AWeapon_Base::GetDamageByDistance(float Distance) const
{
	if (DamageFallOffCurve)
	{
		return BaseDamage * DamageFallOffCurve->GetFloatValue(Distance);
	}
	return BaseDamage;
}

UAbilitySystemComponent* AWeapon_Base::GetOwnerASC() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}
