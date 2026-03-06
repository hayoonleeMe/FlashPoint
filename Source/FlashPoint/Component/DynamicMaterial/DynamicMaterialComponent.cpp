// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicMaterialComponent.h"

#include "AbilitySystemComponent.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DynamicMaterialComponent)

UDynamicMaterialComponent::UDynamicMaterialComponent()
{
	bAutoActivate = true;
}

UDynamicMaterialComponent* UDynamicMaterialComponent::CreateComponent_Internal(UObject* Outer, const UClass* ComponentClass, const FName& Name,
	UAbilitySystemComponent* ASC, const TArray<UMeshComponent*>& MeshesToRegister)
{
	// 받은 클래스가 유효하지 않으면 기본 클래스 사용
	if (!ComponentClass)
	{
		ComponentClass = StaticClass();
	}
	
	UDynamicMaterialComponent* Comp = NewObject<UDynamicMaterialComponent>(Outer, ComponentClass, Name);
	
	if (ensure(ASC))
	{
		Comp->AbilitySystemComponent = ASC;
	}
	
	for (UMeshComponent* MeshComponent : MeshesToRegister)
	{
		Comp->RegisterMesh(MeshComponent);
	}
	
	Comp->RegisterComponent();
	return Comp;
}

UDynamicMaterialComponent* UDynamicMaterialComponent::CreateDynamicMaterialComponent(UObject* Outer, TSubclassOf<UDynamicMaterialComponent> ComponentClass, FName Name, UAbilitySystemComponent* ASC, TArray<UMeshComponent*> MeshesToRegister)
{
	return CreateComponent_Internal(Outer, ComponentClass, Name, ASC, MeshesToRegister);
}

UDynamicMaterialComponent* UDynamicMaterialComponent::CreateComponent(UObject* Outer, const FName& Name, UAbilitySystemComponent* ASC,
	const TArray<UMeshComponent*>& MeshesToRegister)
{
	return CreateComponent_Internal(Outer, StaticClass(), Name, ASC, MeshesToRegister);
}

void UDynamicMaterialComponent::RegisterMesh(UMeshComponent* MeshComponent, bool bSupportedMaterialOnly)
{
	if (MeshComponent)
	{
		bool bMIDsChanged = false;
		
		for (int32 Index = 0; Index < MeshComponent->GetNumMaterials(); ++Index)
		{
			UMaterialInterface* Material = MeshComponent->GetMaterial(Index);
			
			// Index 머터리얼이 이미 MID인지 먼저 체크
			bool bShouldSetMaterial = false;
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(Material);
			if (!MID)
			{
				MID = UMaterialInstanceDynamic::Create(Material, MeshComponent);
				bShouldSetMaterial = true;
			}
			
			if (MID && (!bSupportedMaterialOnly || IsSupportedForMatParam(MID)))
			{
				if (bShouldSetMaterial)
				{
					MeshComponent->SetMaterial(Index, MID);
				}
				MIDs.AddUnique(MID);
				
				bMIDsChanged = true;
				SyncCachedParameters(MID);
			}
		}
		
		if (bMIDsChanged)
		{
			UpdateMaterialActivation();
		}
	}
}

void UDynamicMaterialComponent::UnRegisterMesh(UMeshComponent* MeshComponent)
{
	if (MeshComponent)
	{
		bool bMIDsChanged = false;
		
		for (int32 Index = 0; Index < MeshComponent->GetNumMaterials(); ++Index)
		{
			if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(Index)))
			{
				MIDs.RemoveSingleSwap(MID);
				bMIDsChanged = true;
			}
		}
		
		if (bMIDsChanged)
		{
			UpdateMaterialActivation();
		}
	}
}

void UDynamicMaterialComponent::SetScalarParameterValue(const FName& MatParamName, float Value)
{
	if (const FMaterialParameterConfig* ParamConfig = ScalarMatParamConfigs.FindByKey(MatParamName))
	{
		if (ParamConfig->bMaintainState)
		{
			CachedScalarValues.Add(MatParamName, Value);
		}
		
		for (UMaterialInstanceDynamic* MID : MIDs)
		{
			if (MID)
			{
				MID->SetScalarParameterValue(ParamConfig->MatParamName, Value);
			}
		}
	}
}

void UDynamicMaterialComponent::SetVectorParameterValue(const FName& MatParamName, const FVector& Value)
{
	if (const FMaterialParameterConfig* ParamConfig = VectorMatParamConfigs.FindByKey(MatParamName))
	{
		if (ParamConfig->bMaintainState)
		{
			CachedVectorValues.Add(MatParamName, Value);
		}
		
		for (UMaterialInstanceDynamic* MID : MIDs)
		{
			if (MID)
			{
				MID->SetVectorParameterValue(ParamConfig->MatParamName, Value);
			}
		}
	}
}

void UDynamicMaterialComponent::AddScalarMatParamConfig(const FMaterialParameterConfig& MatParamConfig)
{
	ScalarMatParamConfigs.AddUnique(MatParamConfig);
}

void UDynamicMaterialComponent::AddVectorMatParamConfig(const FMaterialParameterConfig& MatParamConfig)
{
	VectorMatParamConfigs.AddUnique(MatParamConfig);
}

void UDynamicMaterialComponent::AddRequiredTags(const FGameplayTagContainer& InRequiredTags)
{
	if (!AbilitySystemComponent || !InRequiredTags.IsValid())
	{
		return;
	}
	
	FOnGameplayEffectTagCountChanged::FDelegate Delegate;
	Delegate.BindUObject(this, &ThisClass::OnRequiredTagChanged);

	for (const FGameplayTag& Tag : InRequiredTags)
	{
		FDelegateHandle Handle = AbilitySystemComponent->RegisterAndCallGameplayTagEvent(Tag, Delegate);
		RequiredTagEventHandles.Add(Tag, Handle);
	}
	
	RequiredTags.AppendTags(InRequiredTags);
	
	UpdateMaterialActivation();
}

void UDynamicMaterialComponent::RemoveRequiredTags(const FGameplayTagContainer& InRequiredTags)
{
	if (!AbilitySystemComponent || !InRequiredTags.IsValid())
	{
		return;
	}
	
	for (const FGameplayTag& Tag : InRequiredTags)
	{
		FDelegateHandle Handle;
		if (RequiredTagEventHandles.RemoveAndCopyValue(Tag, Handle))
		{
			AbilitySystemComponent->UnregisterGameplayTagEvent(Handle, Tag);
		}
	}
	
	RequiredTags.RemoveTags(InRequiredTags);
	
	UpdateMaterialActivation();
}

void UDynamicMaterialComponent::AddMaterialActivationConfig(const FMaterialActivationConfig& ActivationConfig)
{
	// 이미 등록되어 있으면 종료
	if (MaterialActivationConfigs.Find(ActivationConfig))
	{
		return;
	}
	
	// Scalar Param이 아니면 종료
	if (ActivationConfig.bIsScalarParam && !ScalarMatParamConfigs.FindByKey(ActivationConfig.MatParamName))
	{
		return;
	}
	// Vector Param이 아니면 종료
	if (!ActivationConfig.bIsScalarParam && !VectorMatParamConfigs.FindByKey(ActivationConfig.MatParamName))
	{
		return;
	}
	
	MaterialActivationConfigs.Add(ActivationConfig);
	
	UpdateMaterialActivation();
}

void UDynamicMaterialComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	RegisterRequiredTagEvents();
	UpdateMaterialActivation();
}

void UDynamicMaterialComponent::Deactivate()
{
	Super::Deactivate();
	
	UnRegisterRequiredTagEvents();
	UpdateMaterialActivation();
}

void UDynamicMaterialComponent::BeginPlay()
{
	Super::BeginPlay();

	// 초기 RequiredTags 추가
	if (!DefaultRequiredTags.IsEmpty())
	{
		AddRequiredTags(DefaultRequiredTags);
	}
	
	UpdateMaterialActivation();
}

void UDynamicMaterialComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister all required tags event
	if (AbilitySystemComponent)
	{
		for (const auto& Pair : RequiredTagEventHandles)
		{
			AbilitySystemComponent->UnregisterGameplayTagEvent(Pair.Value, Pair.Key);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UDynamicMaterialComponent::SyncCachedParameters(UMaterialInstanceDynamic* MID) const
{
	if (MID)
	{
		for (const auto& Pair : CachedScalarValues)
		{
			if (const FMaterialParameterConfig* Config = ScalarMatParamConfigs.FindByKey(Pair.Key))
			{
				MID->SetScalarParameterValue(Config->MatParamName, Pair.Value);
			}
		}
		
		for (const auto& Pair : CachedVectorValues)
		{
			if (const FMaterialParameterConfig* Config = VectorMatParamConfigs.FindByKey(Pair.Key))
			{
				MID->SetVectorParameterValue(Config->MatParamName, Pair.Value);
			}
		}
	}
}

bool UDynamicMaterialComponent::IsSupportedForMatParam(const UMaterialInstanceDynamic* MID)
{
	if (MID)
	{
		if (!ScalarMatParamConfigs.IsEmpty())
		{
			float Temp;
			return MID->GetScalarParameterDefaultValue(ScalarMatParamConfigs[0].MatParamName, Temp);
		}
	
		if (!VectorMatParamConfigs.IsEmpty())
		{
			FLinearColor Temp;
			return MID->GetVectorParameterDefaultValue(VectorMatParamConfigs[0].MatParamName, Temp);
		}	
	}
	
	return false;
}

void UDynamicMaterialComponent::OnRequiredTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	UpdateMaterialActivation();
}

void UDynamicMaterialComponent::RegisterRequiredTagEvents()
{
	AddRequiredTags(RequiredTags);
}

void UDynamicMaterialComponent::UnRegisterRequiredTagEvents()
{
	RemoveRequiredTags(RequiredTags);
}

bool UDynamicMaterialComponent::CanActivateMaterial() const
{
	if (RequiredTags.IsValid())
	{
		return AbilitySystemComponent && AbilitySystemComponent->HasAllMatchingGameplayTags(RequiredTags);
	}
	return true;	
}

void UDynamicMaterialComponent::UpdateMaterialActivation()
{
	if (MaterialActivationConfigs.IsEmpty())
	{
		return;
	}
	
	bool bShouldActivateMaterial = IsActive() && CanActivateMaterial();
	
	if (bMaterialActivated != bShouldActivateMaterial)
	{
		for (const FMaterialActivationConfig& Config : MaterialActivationConfigs)
		{
			if (Config.bIsScalarParam)
			{
				const float TargetValue = bShouldActivateMaterial ? Config.ActivationScalarValue : Config.DeactivationScalarValue;
				SetScalarParameterValue(Config.MatParamName, TargetValue);
			}
			else
			{
				const FVector TargetValue = bShouldActivateMaterial ? Config.ActivationVectorValue : Config.DeactivationVectorValue;
				SetVectorParameterValue(Config.MatParamName, TargetValue);
			}
		}
		
		bMaterialActivated = bShouldActivateMaterial;
	}
}

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult UDynamicMaterialComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	for (int32 Index = 0; Index < ScalarMatParamConfigs.Num(); ++Index)
	{
		if (ScalarMatParamConfigs[Index].MatParamName.IsNone())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("[ScalarMatParamConfigs] MatParamName is none for index %d"), Index)));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	for (int32 Index = 0; Index < VectorMatParamConfigs.Num(); ++Index)
	{
		if (VectorMatParamConfigs[Index].MatParamName.IsNone())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("[VectorMatParamConfigs] MatParamName is none for index %d"), Index)));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	for (int32 Index = 0; Index < MaterialActivationConfigs.Num(); ++Index)
	{
		if (MaterialActivationConfigs[Index].MatParamName.IsNone())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("[MaterialActivationConfigs] MatParamName is none for index %d"), Index)));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	return Result;
}
#endif
