// Fill out your copyright notice in the Description page of Project Settings.


#include "Attachment_Scope.h"

#include "Component/DynamicMaterial/ScopeLensMaskComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Attachment_Scope)

AAttachment_Scope::AAttachment_Scope()
{
	ADSMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ADS Mesh Component"));
	ADSMesh->SetupAttachment(RootComponent);
	ADSMesh->SetCastShadow(false);
	ADSMesh->SetOnlyOwnerSee(true);
	ADSMesh->SetHiddenInGame(true);
	
	LensMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lens Mesh Component"));
	LensMesh->SetupAttachment(ADSMesh);
	LensMesh->SetCastShadow(false);
	LensMesh->SetOnlyOwnerSee(true);
	LensMesh->SetHiddenInGame(true);
}

void AAttachment_Scope:: OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	// 부착 관계 재정립
	ADSMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, ADSMeshAttachSocketName);
	LensMesh->AttachToComponent(ADSMesh, FAttachmentTransformRules::KeepRelativeTransform, LensMeshAttachSocketName);
}

void AAttachment_Scope::OnWeaponEquipped()
{
	if (ScopeLensMaskComponent)
	{
		ScopeLensMaskComponent->Activate();
	}
}

void AAttachment_Scope::OnWeaponUnEquipped()
{
	if (ScopeLensMaskComponent)
	{
		ScopeLensMaskComponent->Deactivate();
	}
}

void AAttachment_Scope::StartAimDownSight()
{
	PrimaryMesh->SetHiddenInGame(true);
	ADSMesh->SetHiddenInGame(false);
	LensMesh->SetHiddenInGame(false);
}

void AAttachment_Scope::StopAimDownSight()
{
	PrimaryMesh->SetHiddenInGame(false);
	ADSMesh->SetHiddenInGame(true);
	LensMesh->SetHiddenInGame(true);
}

void AAttachment_Scope::BeginPlay()
{
	Super::BeginPlay();
	
	// 로컬 클라이언트에서 ScopeLensMaskComponent 등록
	if (IsPlayerLocallyControlled() && !ScopeLensMaskComponent)
	{
		ScopeLensMaskComponent = UScopeLensMaskComponent::CreateComponent(this, TEXT("ScopeLensMaskComponent"), GetASC(), { ADSMesh });
		
		// ADSMesh 기준 로컬 렌즈 위치 등록
		const FTransform LensTransform = ADSMesh->GetSocketTransform(LensMeshAttachSocketName, RTS_Component);
		ScopeLensMaskComponent->SetLocalLensLocation(LensTransform.GetLocation());
	}
}
