// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Component/FPActorComponent.h"
#include "DynamicMaterialComponent.generated.h"

class UAbilitySystemComponent;

/**
 * 컴포넌트에서 관리할 머터리얼 파라미터의 정보를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FMaterialParameterConfig
{
	GENERATED_BODY()
	
	// 머터리얼 파라미터 이름
	UPROPERTY(EditDefaultsOnly)
	FName MatParamName;
	
	// 런타임에 값이 변경되었을 때, 나중에 추가되는 MID에도 이 값을 동기화할지 여부
	UPROPERTY(EditDefaultsOnly)
	bool bMaintainState = true;
	
	bool operator==(const FMaterialParameterConfig& Other) const
	{
		return MatParamName == Other.MatParamName;
	}
	
	bool operator==(const FName& OtherName) const
	{
		return MatParamName == OtherName;
	}
};

/**
 * 컴포넌트 활성/비활성 시 함께 업데이트될 머터리얼의 정보를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FMaterialActivationConfig
{
	GENERATED_BODY()
	
	FMaterialActivationConfig() = default;
	
	FMaterialActivationConfig(const FName& InMatParamName, float InActivationScalarValue, float InDeactivationScalarValue)
		: MatParamName(InMatParamName), bIsScalarParam(true), ActivationScalarValue(InActivationScalarValue), DeactivationScalarValue(InDeactivationScalarValue)
	{ }
	
	FMaterialActivationConfig(const FName& InMatParamName, const FVector& InActivationVectorValue, const FVector& InDeactivationVectorValue)
		: MatParamName(InMatParamName), bIsScalarParam(false), ActivationVectorValue(InActivationVectorValue), DeactivationVectorValue(InDeactivationVectorValue)
	{ }
	
	// 머터리얼 파라미터 이름
	UPROPERTY(EditDefaultsOnly)
	FName MatParamName;

	// 스칼라 머터리얼 파라미터인지
	UPROPERTY(EditDefaultsOnly)
	bool bIsScalarParam = false;
	
	// 활성 시 적용할 스칼라 파라미터 값
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bIsScalarParam", EditConditionHides))
	float ActivationScalarValue = 0.f;
	
	// 비활성 시 적용할 스칼라 파라미터 값
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bIsScalarParam", EditConditionHides))
	float DeactivationScalarValue = 0.f;
	
	// 활성 시 적용할 벡터 파라미터 값
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="!bIsScalarParam", EditConditionHides))
	FVector ActivationVectorValue = FVector::ZeroVector;
	
	// 비활성 시 적용할 벡터 파라미터 값
	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="!bIsScalarParam", EditConditionHides))
	FVector DeactivationVectorValue = FVector::ZeroVector;
	
	bool operator==(const FMaterialActivationConfig& Other) const
	{
		return MatParamName == Other.MatParamName;
	}
	
	bool operator==(const FName& OtherName) const
	{
		return MatParamName == OtherName;
	}
};

/**
 * 등록한 메시의 머터리얼을 MID로 등록해 관리하는 컴포넌트
 * 머터리얼 파라미터를 함께 관리
 * 컴포넌트 활성/비활성 시 태그 조건에 따라 특정 파라미터를 자동으로 업데이트
 * 나중에 추가된 메시의 MID에 특정 파라미터 값을 동기화
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UDynamicMaterialComponent : public UFPActorComponent
{
	GENERATED_BODY()

public:
	UDynamicMaterialComponent();
	
protected:
	// 새로운 컴포넌트를 생성하고 등록한 뒤 반환한다.
	// 실제 로직
	static UDynamicMaterialComponent* CreateComponent_Internal(
		UObject* Outer,
		const UClass* ComponentClass,
		const FName& Name,
		UAbilitySystemComponent* ASC,
		const TArray<UMeshComponent*>& MeshesToRegister
	);
	
public:
	// 새로운 컴포넌트를 생성하고 등록한 뒤 반환한다.
	// For Blueprint
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf="Outer", DeterminesOutputType="ComponentClass"))
	static UDynamicMaterialComponent* CreateDynamicMaterialComponent(
		UObject* Outer,
		TSubclassOf<UDynamicMaterialComponent> ComponentClass,
		FName Name,
		UAbilitySystemComponent* ASC,
		TArray<UMeshComponent*> MeshesToRegister
	);
	
	// 새로운 컴포넌트를 생성하고 등록한 뒤 반환한다.
	// For Native Code
	static UDynamicMaterialComponent* CreateComponent(
		UObject* Outer,
		const FName& Name,
		UAbilitySystemComponent* ASC,
		const TArray<UMeshComponent*>& MeshesToRegister
	);

	// MeshComponent의 머터리얼을 MID로 등록한다.
	// bSupportedMaterialOnly가 true면, 컴포넌트에 등록된 Material Parameter가 존재할 때만 등록한다.
	UFUNCTION(BlueprintCallable)
	void RegisterMesh(UMeshComponent* MeshComponent, bool bSupportedMaterialOnly = true);
	
	// MeshComponent의 머터리얼을 등록 해제한다.
	// 기존 머터리얼로 재설정하지 않는다.
	UFUNCTION(BlueprintCallable)
	void UnRegisterMesh(UMeshComponent* MeshComponent);
	
	// MatParamName에 해당하는 Scalar Material Parameter의 값을 Value로 설정한다.
	UFUNCTION(BlueprintCallable)
	void SetScalarParameterValue(const FName& MatParamName, float Value);

	// MatParamName에 해당하는 Vector Material Parameter의 값을 Value로 설정한다.
	UFUNCTION(BlueprintCallable)
	void SetVectorParameterValue(const FName& MatParamName, const FVector& Value);
	
	// 컴포넌트에 Scalar Material Parameter를 등록한다.
	UFUNCTION(BlueprintCallable)
	void AddScalarMatParamConfig(const FMaterialParameterConfig& MatParamConfig);
	
	// 컴포넌트에 Vector Material Parameter를 등록한다.
	UFUNCTION(BlueprintCallable)
	void AddVectorMatParamConfig(const FMaterialParameterConfig& MatParamConfig);
	
	// Material Activation 태그 조건을 추가한다.
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="InRequiredTags"))
	void AddRequiredTags(const FGameplayTagContainer& InRequiredTags);
	
	// Material Activation 태그 조건을 제거한다.
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="InRequiredTags"))
	void RemoveRequiredTags(const FGameplayTagContainer& InRequiredTags);

	// 컴포넌트 활성/비활성 시 함께 업데이트될 머터리얼 정보를 추가한다.
	void AddMaterialActivationConfig(const FMaterialActivationConfig& ActivationConfig);
	
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 등록된 메시들의 Material Instance Dynamic 배열
	UPROPERTY(VisibleInstanceOnly, Category="Material")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MIDs;
	
	// 변경된 스칼라 파라미터를 캐싱하는 맵
	UPROPERTY(Transient, VisibleInstanceOnly, Category="Material|Parameter")
	TMap<FName, float> CachedScalarValues;
	
	// 변경된 벡터 파라미터를 캐싱하는 맵
	UPROPERTY(Transient, VisibleInstanceOnly, Category="Material|Parameter")
	TMap<FName, FVector> CachedVectorValues;
	
	// MID에 현재 캐싱된 파라미터를 동기화 한다.
	void SyncCachedParameters(UMaterialInstanceDynamic* MID) const;

	// 컴포넌트에서 MID에 적용할 Scalar Material Parameter
	UPROPERTY(EditDefaultsOnly, Category="Material|Parameter")
	TArray<FMaterialParameterConfig> ScalarMatParamConfigs;
	
	// 컴포넌트에서 MID에 적용할 Vector Material Parameter
	UPROPERTY(EditDefaultsOnly, Category="Material|Parameter")
	TArray<FMaterialParameterConfig> VectorMatParamConfigs;
	
	// MID가 컴포넌트에 등록된 Material Parameter Info를 지원하는지 체크
	bool IsSupportedForMatParam(const UMaterialInstanceDynamic* MID);

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// 기본으로 등록될 Material Activation 태그 조건
	UPROPERTY(EditDefaultsOnly, Category="Material|Activation")
	FGameplayTagContainer DefaultRequiredTags;

	// Material Activation 태그 조건
	UPROPERTY(VisibleInstanceOnly, Category="Material|Activation")
	FGameplayTagContainer RequiredTags;
	
	// 등록된 Tag Event Handle
	TMap<FGameplayTag, FDelegateHandle> RequiredTagEventHandles;
	
	void OnRequiredTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	// RequiredTags Event를 등록한다.
	void RegisterRequiredTagEvents();
	
	// RequiredTags Event를 등록 해제한다.
	void UnRegisterRequiredTagEvents();
	
	// 컴포넌트 활성/비활성 시 함께 업데이트될 머터리얼 정보
	UPROPERTY(EditDefaultsOnly, Category="Material|Activation")
	TArray<FMaterialActivationConfig> MaterialActivationConfigs;
	
	// 컴포넌트 활성/비활성 시 등록된 머터리얼이 함께 업데이트될 수 있는지 체크
	virtual bool CanActivateMaterial() const;
	
	// 컴포넌트 활성/비활성 시 등록된 머터리얼을 함께 업데이트
	void UpdateMaterialActivation();
	
	// 현재 머터리얼의 활성화 상태
	bool bMaterialActivated = false;
	
public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
