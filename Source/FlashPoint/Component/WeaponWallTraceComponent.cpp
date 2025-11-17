// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWallTraceComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponWallTraceComponent)

UWeaponWallTraceComponent::UWeaponWallTraceComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;	// BeginPlay에서 로컬에서만 활성화
	
	WallTraceRadius = 8.f;
	WeaponUpPitchThreshold = 10.f;
	WallTraceObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic) };
	WallTraceStartUpOffset = 60.f;
	WallTraceStartRightOffset = 10.f;
	WallTraceStartForwardOffsetOutRange = { 30.f, -15.f };
	WallTraceLengthMultiplierOutRange = { 1.f, 1.3f };
}

void UWeaponWallTraceComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UWeaponWallTraceComponent, bIsWeaponWallBlocked, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UWeaponWallTraceComponent, bUseWeaponUp, COND_SkipOwner);
}

void UWeaponWallTraceComponent::RetrieveWeaponWallBlockData(bool& bOutIsWeaponWallBlocked, bool& bOutUseWeaponUp, FVector& OutLocalWallHitLocation) const
{
	bOutIsWeaponWallBlocked = bIsWeaponWallBlocked;
	bOutUseWeaponUp = bUseWeaponUp;
	OutLocalWallHitLocation = LocalWallHitLocation;
}

void UWeaponWallTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	const APawn* Pawn = GetOwner<APawn>();
	if (Pawn && Pawn->IsLocallyControlled())
	{
		// Only Tick in local
		SetComponentTickEnabled(true);
		
		if (UWeaponManageComponent* WeaponManageComp = Pawn->FindComponentByClass<UWeaponManageComponent>())
		{
			WeaponManageComp->OnEquippedWeaponChanged.AddUObject(this, &ThisClass::UpdateEquippedWeapon);
		}
	}
}

void UWeaponWallTraceComponent::UpdateEquippedWeapon(AWeapon_Base* InEquippedWeapon)
{
	EquippedWeapon = InEquippedWeapon;
}

void UWeaponWallTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only Called in Local Client
	if (const APawn* Pawn = GetOwner<APawn>())
	{
		if (EquippedWeapon)
		{
			LocalWallTrace(Pawn);
		}
	}
}

void UWeaponWallTraceComponent::LocalWallTrace(const APawn* Pawn)
{
	const FVector PawnLoc = Pawn->GetActorLocation();
	const float AimPitch = UKismetMathLibrary::NormalizeAxis(Pawn->GetBaseAimRotation().Pitch);

	// 시점을 상하로 회전시킬 때 Trace 궤적이 몸에 파묻히지 않도록 시작점에 Offset 적용 
	const float ForwardOffset = FMath::GetMappedRangeValueClamped({ -90.f, 90.f }, WallTraceStartForwardOffsetOutRange, AimPitch);
	const FVector Start = PawnLoc + FVector(0.f, 0.f, WallTraceStartUpOffset) + Pawn->GetActorForwardVector() * ForwardOffset + Pawn->GetActorRightVector() * WallTraceStartRightOffset;
	// 시점이 위, 아래인 경우 Length 보정
	const float WallTraceLength = EquippedWeapon->GetWallTraceLength() * FMath::GetMappedRangeValueClamped({ 0.f, 90.f }, WallTraceLengthMultiplierOutRange, FMath::Abs(AimPitch));
	// 시점을 따라 Trace
	const FVector End = Start + Pawn->GetControlRotation().Vector() * WallTraceLength;

	FHitResult HitResult;
	const bool bNewIsWeaponWallBlocked = UKismetSystemLibrary::SphereTraceSingleForObjects(Pawn, Start, End, WallTraceRadius, WallTraceObjectTypes, false, {}, EDrawDebugTrace::None, HitResult, true);

	const bool bNewUseWeaponUp = AimPitch > WeaponUpPitchThreshold;

	if (!bIsWeaponWallBlocked)
	{
		SetIsWeaponWallBlocked(bNewIsWeaponWallBlocked);
		SetUseWeaponUp(bNewUseWeaponUp);
	}
	else
	{
		if (bUseWeaponUp != bNewUseWeaponUp)
		{
			// Weapon Block 방향이 바뀔 경우 총이 반대로 이동하기 위한 공간이 필요하므로 이를 체크하는 Trace 수행
			// 실제 Sweep을 수행하는 대신 간소화해 총구 전방에서 Wall Trace 끝 지점을 향한 Trace 수행
			const float SpaceTraceForwardOffset = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AimPitch), 0.f, 90.f, 0.f, EquippedWeapon->GetWallTraceLength() * 0.5f);
			const FVector SpaceTraceStart = EquippedWeapon->GetWeaponTargetingSourceLocation() + Pawn->GetActorForwardVector() * SpaceTraceForwardOffset;
			const FVector SpaceTraceEnd = bNewIsWeaponWallBlocked ? HitResult.ImpactPoint : HitResult.TraceEnd;
			FHitResult SpaceTraceHitResult;
			const bool bSpaceTraceHit = UKismetSystemLibrary::SphereTraceSingleForObjects(Pawn, SpaceTraceStart, SpaceTraceEnd, WallTraceRadius, WallTraceObjectTypes, false, {}, EDrawDebugTrace::None, SpaceTraceHitResult, true);
			if (!bSpaceTraceHit)
			{
				SetIsWeaponWallBlocked(bNewIsWeaponWallBlocked);
				SetUseWeaponUp(bNewUseWeaponUp);
			}
		}
		else if (bIsWeaponWallBlocked != bNewIsWeaponWallBlocked)
		{
			// Weapon Block 방향이 바뀌지 않고 Block 여부만 바뀔 경우 바로 변경 적용
			SetIsWeaponWallBlocked(bNewIsWeaponWallBlocked);
			SetUseWeaponUp(bNewUseWeaponUp);
		}
	}

	if (bIsWeaponWallBlocked && bNewIsWeaponWallBlocked)
	{
		LocalWallHitLocation = HitResult.ImpactPoint;
	}
}

void UWeaponWallTraceComponent::SetIsWeaponWallBlocked(bool bInIsWeaponWallBlocked)
{
	if (bIsWeaponWallBlocked != bInIsWeaponWallBlocked)
	{
		bIsWeaponWallBlocked = bInIsWeaponWallBlocked;
		ServerSetIsWeaponWallBlocked(bIsWeaponWallBlocked);
	}
}

void UWeaponWallTraceComponent::ServerSetIsWeaponWallBlocked_Implementation(bool bInIsWeaponWallBlocked)
{
	bIsWeaponWallBlocked = bInIsWeaponWallBlocked;
}

void UWeaponWallTraceComponent::SetUseWeaponUp(bool bInUseWeaponUp)
{
	if (bUseWeaponUp != bInUseWeaponUp)
	{
		bUseWeaponUp = bInUseWeaponUp;
		ServerSetUseWeaponUp(bUseWeaponUp);
	}
}

void UWeaponWallTraceComponent::ServerSetUseWeaponUp_Implementation(bool bInUseWeaponUp)
{
	bUseWeaponUp = bInUseWeaponUp;
}
