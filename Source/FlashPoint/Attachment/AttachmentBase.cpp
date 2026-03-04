// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentBase.h"

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
