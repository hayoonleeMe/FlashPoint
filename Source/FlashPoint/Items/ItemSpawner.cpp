// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawner.h"

#include "FlashPoint.h"
#include "FPLogChannels.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

AItemSpawner::AItemSpawner()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	PadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad Mesh Component"));
	SetRootComponent(PadMeshComponent);
	PadMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetupAttachment(PadMeshComponent);
	SphereComponent->SetCollisionObjectType(ECC_Spawner);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh Component"));
	ItemMeshComponent->SetupAttachment(SphereComponent);
	ItemMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeshRotateSpeed = 20.f;
}

void AItemSpawner::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemSpawner, ActiveItemIndex);
}

void AItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FRotator CurrentRotator = SphereComponent->GetRelativeRotation();
	SphereComponent->SetRelativeRotation(FRotator(CurrentRotator.Pitch, CurrentRotator.Yaw + MeshRotateSpeed * DeltaTime, CurrentRotator.Roll));
}

void AItemSpawner::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
										bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !ItemInfos.IsValidIndex(ActiveItemIndex))
	{
		return;
	}
	
	TSubclassOf<AActor> ItemClass = ItemInfos[ActiveItemIndex].ItemClass;
	if (!ItemClass)
	{
		return;
	}

	if (ItemClass->IsChildOf<AWeapon_Base>())
	{
		if (UWeaponManageComponent* WeaponManageComp = OtherActor->FindComponentByClass<UWeaponManageComponent>())
		{
			TSubclassOf<AWeapon_Base> WeaponClass(ItemClass);
			WeaponManageComp->EquipNewWeapon(WeaponClass);
		}
	}
}

void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (ItemInfos.IsEmpty())
	{
		UE_LOG(LogFP, Warning, TEXT("[%s] Item Classes is empty."), *GetName());
		return;
	}

	if (HasAuthority())
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
		
		GetWorldTimerManager().SetTimer(ItemChangeTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::ChangeItem), ItemChangeDelay, true, 0.f);
	}
}

void AItemSpawner::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(AItemSpawner, ItemInfos))
	{
		if (ItemInfos.IsEmpty())
		{
			SetActorTickEnabled(false);
			ItemMeshComponent->SetStaticMesh(nullptr);
			ItemMeshComponent->SetRelativeTransform(FTransform::Identity);
			SphereComponent->SetSphereRadius(0.f);
			SphereComponent->SetRelativeLocation(FVector::ZeroVector);
		}
		else
		{
			// 첫번째 아이템을 미리보기로 표시한다.
			SetActorTickEnabled(true);
			ItemMeshComponent->SetStaticMesh(ItemInfos[0].ItemMesh);
			ItemMeshComponent->SetRelativeTransform(ItemInfos[0].ItemMeshTransform);
			SphereComponent->SetSphereRadius(ItemInfos[0].SphereRadius);
			SphereComponent->SetRelativeLocation(FVector(0.f, 0.f, ItemInfos[0].SphereRadius));
		}
	}
}

void AItemSpawner::ChangeItem()
{
	// called on server only
	const int32 NumItems = ItemInfos.Num();
	ActiveItemIndex = FMath::RandRange(0, NumItems - 1);
}

void AItemSpawner::OnRep_ActiveItemIndex()
{
	// Show Item Mesh only in client
	if (UStaticMesh* ItemMesh = ItemInfos[ActiveItemIndex].ItemMesh)
	{
		ItemMeshComponent->SetStaticMesh(ItemMesh);	
	}
}
