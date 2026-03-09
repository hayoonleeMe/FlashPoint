// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentBase.h"

#include "AbilitySystemGlobals.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttachmentBase)

AAttachmentBase::AAttachmentBase()
{
	SetActorEnableCollision(false);
	
	PrimaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Primary Mesh Component"));
	SetRootComponent(PrimaryMesh);
}

FTransform AAttachmentBase::GetAttachmentSocketTransform(const FName& SocketName, ERelativeTransformSpace TransformSpace) const
{
	return PrimaryMesh->GetSocketTransform(SocketName, TransformSpace);
}

bool AAttachmentBase::IsPlayerLocallyControlled() const
{
	return GetInstigatorController() && GetInstigatorController()->IsLocalPlayerController();
}

UAbilitySystemComponent* AAttachmentBase::GetASC() const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
	{
		return ASC;
	}
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator()))
	{
		return ASC;
	}
	
	return nullptr;
}
