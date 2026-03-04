// Fill out your copyright notice in the Description page of Project Settings.


#include "FPActorComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPActorComponent)

bool UFPActorComponent::HasAuthority() const
{
	return GetOwnerRole() == ROLE_Authority;
}

bool UFPActorComponent::IsPlayerLocallyControlled() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}
	
	if (const APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		return OwnerPawn->IsLocallyControlled();
	}
	
	if (const APawn* InstigatorPawn = Owner->GetInstigator<APawn>())
	{
		return  InstigatorPawn->IsLocallyControlled();
	}
	
	return false;
}
