// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomFovComponent.h"

#include "FPGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CustomFovComponent)

namespace
{
	const FName CustomFOVName = TEXT("CustomFOV");
	const FName EnableCustomFOVName = TEXT("EnableCustomFOV");
	const FName ScaleInDepthName = TEXT("ScaleInDepth");
	const FName ScreenOffsetName = TEXT("ScreenOffset");
	
}

UCustomFovComponent::UCustomFovComponent()
{
	ScalarMatParamConfigs.Add({ CustomFOVName, true });
	ScalarMatParamConfigs.Add({ EnableCustomFOVName, true });
	ScalarMatParamConfigs.Add({ ScaleInDepthName, true });
	VectorMatParamConfigs.Add({ ScreenOffsetName, true });
	DefaultRequiredTags.AddTag(FPGameplayTags::CharacterState::IsFirstPerson);
	DefaultRequiredTags.AddTag(FPGameplayTags::CharacterState::IsEquippingWeapon);
	MaterialActivationConfigs.Add({ EnableCustomFOVName, 1.f, 0.f });
}

UCustomFovComponent* UCustomFovComponent::CreateComponent(UObject* Outer, const FName& Name, UAbilitySystemComponent* ASC,
	const TArray<UMeshComponent*>& MeshesToRegister)
{
	return Cast<UCustomFovComponent>(CreateComponent_Internal(Outer, StaticClass(), Name, ASC, MeshesToRegister));
}

void UCustomFovComponent::SetCustomFOV(float Value)
{
	SetScalarParameterValue(CustomFOVName, Value);
}

void UCustomFovComponent::SetEnableCustomFOV(float Value)
{
	SetScalarParameterValue(EnableCustomFOVName, Value);
}

void UCustomFovComponent::SetScaleInDepth(float Value)
{
	SetScalarParameterValue(ScaleInDepthName, Value);
}

void UCustomFovComponent::SetScreenOffset(const FVector& Value)
{
	SetVectorParameterValue(ScreenOffsetName, Value);
}
