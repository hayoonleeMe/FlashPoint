// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPAttachmentData.h"
#include "GameplayTagContainer.h"
#include "Component/FPActorComponent.h"
#include "AttachmentManageComponent.generated.h"

class AAttachmentBase;

/**
 * 부착물 슬롯을 나타내는 enum class
 */
UENUM(BlueprintType)
enum class EAttachmentSlot : uint8
{
	UpperRail,
	MAX UMETA(Hidden)
};

/**
 * 생성된 부착물을 관리하는 구조체
 */
USTRUCT()
struct FEquippedAttachment
{
	GENERATED_BODY()

	// 해당 부착물 Data Asset
	UPROPERTY()
	TObjectPtr<UFPAttachmentData> AttachmentData;
	
	// 생성된 부착물 액터
	// 로컬에서만 스폰된다.
	UPROPERTY()
	TObjectPtr<AAttachmentBase> AttachmentActor;

	// 해당 부착물을 나타내는 태그 
	// FAttachmentEquipData와의 비교에 사용한다. 
	FGameplayTag AttachmentTypeTag = FGameplayTag::EmptyTag;
};

/**
 * 부착물 장착에 필요한 데이터를 저장하는 구조체
 */
USTRUCT()
struct FAttachmentEquipData
{
	GENERATED_BODY()
	
	UPROPERTY()
	EAttachmentSlot AttachmentSlot = EAttachmentSlot::MAX;
	
	UPROPERTY()
	FGameplayTag AttachmentTypeTag = FGameplayTag::EmptyTag;
	
	// AttachmentSlot으로만 비교
	bool operator==(const EAttachmentSlot& Other) const
	{
		return AttachmentSlot == Other;
	}
};

/**
 * 부착물이 장착될 때 어떤 본을 숨기고 표시할지 정보를 저장하는 구조체
 */
USTRUCT(BlueprintType)
struct FAttachmentBoneVisibilityConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FName> BonesToHide;
	
	UPROPERTY(EditAnywhere)
	TArray<FName> BonesToShow;
};

/**
 * 부착물을 장착하고 관리하는 컴포넌트
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UAttachmentManageComponent : public UFPActorComponent
{
	GENERATED_BODY()

public:
	UAttachmentManageComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void InitializeComponent() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	// 로컬에서 부착물을 추가한다.
	UFUNCTION(BlueprintCallable, meta=(Categories="Attachment", AutoCreateRefTerm="AttachmentTypeTag"))
	void LocalAddAttachment(EAttachmentSlot AttachmentSlot, const FGameplayTag& AttachmentTypeTag, bool bSendServerRPC = true);
	
	// 서버에서 부착물을 추가하는 RPC
	UFUNCTION(Server, Reliable)
	void ServerAddAttachment(EAttachmentSlot AttachmentSlot, const FGameplayTag& AttachmentTypeTag);
	
	// 로컬에서 부착물을 제거한다.
	UFUNCTION(BlueprintCallable)
	void LocalRemoveAttachment(EAttachmentSlot AttachmentSlot, bool bSendServerRPC = true);
	
	// 서버에서 부착물을 제거하는 RPC
	UFUNCTION(Server, Reliable)
	void ServerRemoveAttachment(EAttachmentSlot AttachmentSlot);
	
	// AttachmentSlot에 생성된 부착물의 Data Asset을 반환한다.
	UFUNCTION(BlueprintCallable)
	UFPAttachmentData* GetAttachmentData(EAttachmentSlot AttachmentSlot) const;
	
	// 현재 장착중인 부착물의 모든 MeshComponent를 OutArray에 추가한다.
	// OutArray를 초기화하지 않고 Add()로 추가한다.
	void GetAllEquippedAttachmentMeshes(TArray<UMeshComponent*>& OutArray) const;
	
	// 부착물 메시의 소켓 트랜스폼을 OutTransform에 저장하고 성공 여부를 반환한다.
	bool GetAttachmentSocketTransform(FTransform& OutTransform, EAttachmentSlot AttachmentSlot, const FName& SocketName, ERelativeTransformSpace TransformSpace = RTS_World) const;
	
	// 부착물 착용 상태가 변경될 때 호출되는 델레게이트
	DECLARE_MULTICAST_DELEGATE_TwoParams(FAttachmentChangedDelegate, EAttachmentSlot /*AttachmentSlot*/, AActor* /*AttachmentActor*/);
	
	// 부착물이 추가될 때 호출되는 델레게이트
	FAttachmentChangedDelegate OnAttachmentAddedDelegate;
	
	// 부착물이 제거될 때 호출되는 델레게이트
	FAttachmentChangedDelegate OnAttachmentRemovedDelegate;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<UMeshComponent> OwnerMeshComponent;
	
	// 현재 슬롯에 장착된 부착물을 관리하는 맵 
	UPROPERTY(Transient, VisibleAnywhere)
	TMap<EAttachmentSlot, FEquippedAttachment> EquippedAttachments;
	
	// 부착물이 추가될 때 호출된다.
	virtual void OnAttachmentAdded(EAttachmentSlot AttachmentSlot, const FEquippedAttachment& EquippedAttachment);
	
	// 부착물이 제거될 때 호출된다.
	virtual void OnAttachmentRemoved(EAttachmentSlot AttachmentSlot, const FEquippedAttachment& EquippedAttachment);
	
	// AttachmentSlot에 장착된 부착물의 T 타입의 스탯 객체를 반환한다.
	template <class T>
	const T* GetAttachmentStat(EAttachmentSlot AttachmentSlot) const
	{
		if (const UFPAttachmentData* AttachmentData = EquippedAttachments.FindRef(AttachmentSlot).AttachmentData)
		{
			return AttachmentData->GetAttachmentStat<T>();
		}
		return nullptr;
	}
	
private:
	// 게임 시작 시 기본으로 장착될 부착물
	UPROPERTY(EditDefaultsOnly, meta=(Categories="Attachment"))
	TMap<EAttachmentSlot, FGameplayTag> DefaultAttachmentData;
	
	// Replicate되는 부착물 장착 정보
	// 로컬 클라이언트에선 직접 업데이트하고, 그 외 클라이언트에선 서버에서 복제된다.
	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedEquipData)
	TArray<FAttachmentEquipData> ReplicatedEquipData;
	
	// EquipData를 추가한다.
	void AddReplicatedEquipData(EAttachmentSlot AttachmentSlot, const FGameplayTag& AttachmentTypeTag);
	
	// EquipData를 제거한다.
	void RemoveReplicatedEquipData(EAttachmentSlot AttachmentSlot);
	
	// 로컬 클라이언트를 제외한 클라이언트로 복제될 때 변경 사항을 업데이트한다.
	UFUNCTION()
	void OnRep_ReplicatedEquipData();
	
	// AttachmentSlot의 AttachmentTypeTag 타입의 부착물을 장착한다.
	// 이미 슬롯에 다른 타입의 부착물이 존재하면 제거하고 새로 장착한다.
	void AddAttachment_Internal(EAttachmentSlot AttachmentSlot, const FGameplayTag& AttachmentTypeTag);
	
	// 장착 중인 부착물 중 AttachmentSlot의 부착물을 제거한다.
	void RemoveAttachment_Internal(EAttachmentSlot AttachmentSlot);

	// 특정 슬롯에 부착물이 장착될 때 OwnerMesh에서 어떤 본을 숨기고 표시할지를 나타내는 컨테이너
	UPROPERTY(EditDefaultsOnly)
	TMap<EAttachmentSlot, FAttachmentBoneVisibilityConfig> BoneVisibilityConfigs;

	// OwnerMesh Bone Visibility를 업데이트한다.
	void UpdateOwnerMeshBoneVisibility(EAttachmentSlot AttachmentSlot, bool bAttachmentAdded) const;
	
public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
