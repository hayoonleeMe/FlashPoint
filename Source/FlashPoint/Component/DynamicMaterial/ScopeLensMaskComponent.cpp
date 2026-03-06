// Fill out your copyright notice in the Description page of Project Settings.


#include "ScopeLensMaskComponent.h"

#include "FPGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ScopeLensMaskComponent)

namespace
{
	const FName EnableScopeLensMaskName = TEXT("EnableScopeLensMask");
	const FName LensRadiusName = TEXT("LensRadius");
	const FName LocalLensLocationName = TEXT("LocalLensLocation");
}

UScopeLensMaskComponent::UScopeLensMaskComponent()
{
	ScalarMatParamConfigs.Add({ EnableScopeLensMaskName, true });
	ScalarMatParamConfigs.Add({ LensRadiusName, true });
	VectorMatParamConfigs.Add({ LocalLensLocationName, true });
	DefaultRequiredTags.AddTag(FPGameplayTags::CharacterState::IsAimingDownSight);
	MaterialActivationConfigs.Add({ EnableScopeLensMaskName, 1.f, 0.f });
}

UScopeLensMaskComponent* UScopeLensMaskComponent::CreateComponent(UObject* Outer, const FName& Name, UAbilitySystemComponent* ASC,
	const TArray<UMeshComponent*>& MeshesToRegister)
{
	return Cast<UScopeLensMaskComponent>(CreateComponent_Internal(Outer, StaticClass(), Name, ASC, MeshesToRegister));
}

void UScopeLensMaskComponent::SetEnableScopeLensMask(float Value)
{
	SetScalarParameterValue(EnableScopeLensMaskName, Value);
}

void UScopeLensMaskComponent::SetLensRadius(float Value)
{
	SetScalarParameterValue(LensRadiusName, Value);
}

void UScopeLensMaskComponent::SetLocalLensLocation(const FVector& Value)
{
	SetVectorParameterValue(LocalLensLocationName, Value);
}
