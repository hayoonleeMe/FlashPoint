// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentManageComponent.h"

#include "AttachmentBase.h"
#include "AttachmentOwnerInterface.h"
#include "FPAttachmentData.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttachmentManageComponent)

UAttachmentManageComponent::UAttachmentManageComponent()
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UAttachmentManageComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// 로컬 클라이언트에선 직접 업데이트하고, 그 외 클라이언트에선 서버에서 복제된다. 
	DOREPLIFETIME_CONDITION(UAttachmentManageComponent, ReplicatedEquipData, COND_SkipOwner);
}

void UAttachmentManageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Caching Owner Mesh Component
	IAttachmentOwnerInterface* AttachmentOwnerInterface = Cast<IAttachmentOwnerInterface>(GetOwner());
	if (ensure(AttachmentOwnerInterface))
	{
		OwnerMeshComponent = AttachmentOwnerInterface->GetAttachmentOwnerMeshComponent();
	}
}

void UAttachmentManageComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	// 컴포넌트가 제거될 때 생성된 모든 부착물 액터를 제거한다.
	for (const auto& Pair : EquippedAttachments)
	{
		AAttachmentBase* AttachmentActor = Pair.Value.AttachmentActor;
		if (IsValid(AttachmentActor))
		{
			AttachmentActor->Destroy();
		}
	}
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UAttachmentManageComponent::LocalAddAttachment(EAttachmentSlot AttachmentSlot, const FGameplayTag& AttachmentTypeTag, bool bSendServerRPC)
{
	// Local Predicted하게 먼저 생성
	AddReplicatedEquipData(AttachmentSlot, AttachmentTypeTag);
	AddAttachment_Internal(AttachmentSlot, AttachmentTypeTag);
	
	if (bSendServerRPC)
	{
		ServerAddAttachment(AttachmentSlot, AttachmentTypeTag);
	}
}

void UAttachmentManageComponent::ServerAddAttachment_Implementation(EAttachmentSlot AttachmentSlot, const FGameplayTag& AttachmentTypeTag)
{
	// Owner를 제외한 Simulated Proxy에서 Rep Notify 호출
	AddReplicatedEquipData(AttachmentSlot, AttachmentTypeTag);
	AddAttachment_Internal(AttachmentSlot, AttachmentTypeTag);
}

void UAttachmentManageComponent::LocalRemoveAttachment(EAttachmentSlot AttachmentSlot, bool bSendServerRPC)
{
	// Local Predicted하게 먼저 제거
	RemoveReplicatedEquipData(AttachmentSlot);
	RemoveAttachment_Internal(AttachmentSlot);
	
	if (bSendServerRPC)
	{
		ServerRemoveAttachment(AttachmentSlot);
	}
}

void UAttachmentManageComponent::ServerRemoveAttachment_Implementation(EAttachmentSlot AttachmentSlot)
{
	// Owner를 제외한 Simulated Proxy에서 Rep Notify 호출
	RemoveReplicatedEquipData(AttachmentSlot);
	RemoveAttachment_Internal(AttachmentSlot);
}

UFPAttachmentData* UAttachmentManageComponent::GetAttachmentData(EAttachmentSlot AttachmentSlot) const
{
	return EquippedAttachments.FindRef(AttachmentSlot).AttachmentData;
}

AActor* UAttachmentManageComponent::GetAttachmentActor(EAttachmentSlot AttachmentSlot) const
{
	return EquippedAttachments.FindRef(AttachmentSlot).AttachmentActor;
}

void UAttachmentManageComponent::GetAllEquippedAttachmentMeshes(TArray<UMeshComponent*>& OutArray) const
{
	for (const auto& Pair : EquippedAttachments)
	{
		if (AAttachmentBase* AttachmentActor = Pair.Value.AttachmentActor)
		{
			AttachmentActor->ForEachComponent<UMeshComponent>(false, [&](UMeshComponent* Component)
			{
				OutArray.Add(Component);
			});
		}
	}
}

bool UAttachmentManageComponent::GetAttachmentSocketTransform(FTransform& OutTransform, EAttachmentSlot AttachmentSlot, const FName& SocketName, ERelativeTransformSpace TransformSpace) const
{
	if (const AAttachmentBase* AttachmentActor = EquippedAttachments.FindRef(AttachmentSlot).AttachmentActor)
	{
		OutTransform = AttachmentActor->GetAttachmentSocketTransform(SocketName, TransformSpace);
		return true;
	}
	return false;
}

void UAttachmentManageComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Add Default Attachment
	for (const auto& Pair : DefaultAttachmentData)
	{
		const EAttachmentSlot AttachmentSlot = Pair.Key;
		const FGameplayTag& AttachmentTypeTag = Pair.Value;
		
		if (HasAuthority())
		{
			ServerAddAttachment_Implementation(AttachmentSlot, AttachmentTypeTag);
		}
		else if (IsPlayerLocallyControlled())
		{
			LocalAddAttachment(AttachmentSlot, AttachmentTypeTag, false);
		}
	}
}

void UAttachmentManageComponent::OnAttachmentAdded(EAttachmentSlot AttachmentSlot, const FEquippedAttachment& EquippedAttachment)
{
	if (OnAttachmentAddedDelegate.IsBound())
	{
		OnAttachmentAddedDelegate.Broadcast(AttachmentSlot, EquippedAttachment.AttachmentActor);
	}
	
	UpdateOwnerMeshBoneVisibility(AttachmentSlot, true);
}

void UAttachmentManageComponent::OnAttachmentRemoved(EAttachmentSlot AttachmentSlot, const FEquippedAttachment& EquippedAttachment)
{
	if (OnAttachmentRemovedDelegate.IsBound())
	{
		OnAttachmentRemovedDelegate.Broadcast(AttachmentSlot, EquippedAttachment.AttachmentActor);
	}
	
	UpdateOwnerMeshBoneVisibility(AttachmentSlot, false);
}

void UAttachmentManageComponent::AddReplicatedEquipData(EAttachmentSlot AttachmentSlot, const FGameplayTag& AttachmentTypeTag)
{
	if (FAttachmentEquipData* EquipData = ReplicatedEquipData.FindByKey(AttachmentSlot))
	{
		EquipData->AttachmentTypeTag = AttachmentTypeTag;
	}
	else
	{
		ReplicatedEquipData.Add({ AttachmentSlot, AttachmentTypeTag });
	}
}

void UAttachmentManageComponent::RemoveReplicatedEquipData(EAttachmentSlot AttachmentSlot)
{
	if (FAttachmentEquipData* EquipData = ReplicatedEquipData.FindByKey(AttachmentSlot))
	{
		// 배열에서 제거하지 않고 타입 태그만 변경
		EquipData->AttachmentTypeTag = FGameplayTag::EmptyTag;
	}
}

void UAttachmentManageComponent::OnRep_ReplicatedEquipData()
{
	// Simulated Proxy에 부착물 업데이트
	for (const FAttachmentEquipData& EquipData : ReplicatedEquipData)
	{
		if (EquipData.AttachmentTypeTag.IsValid())
		{
			// 부착물 생성
			AddAttachment_Internal(EquipData.AttachmentSlot, EquipData.AttachmentTypeTag);
		}
		else
		{
			// 부착물 제거
			RemoveAttachment_Internal(EquipData.AttachmentSlot);
		}
	}
}

void UAttachmentManageComponent::AddAttachment_Internal(EAttachmentSlot AttachmentSlot, const FGameplayTag& AttachmentTypeTag)
{
	if (!ensure(OwnerMeshComponent))
	{
		return;
	}
	
	UFPAttachmentData* AttachmentData = UFPAttachmentData::Get(AttachmentTypeTag);
	if (!AttachmentData || !AttachmentData->AttachmentClass)
	{
		return;
	}
	
	FEquippedAttachment& EquippedAttachment = EquippedAttachments.FindOrAdd(AttachmentSlot);
	
	if (IsValid(EquippedAttachment.AttachmentActor))
	{
		if (AttachmentTypeTag.MatchesTagExact(EquippedAttachment.AttachmentTypeTag))
		{
			// 이미 동일한 부착물이면 early return
			return;
		}
		
		// 다른 종류의 부착물이 존재하면 제거
		EquippedAttachment.AttachmentActor->Destroy();
		EquippedAttachment.AttachmentActor = nullptr;
	}
	
	EquippedAttachment.AttachmentTypeTag = AttachmentTypeTag;
	EquippedAttachment.AttachmentData = AttachmentData;
	
	// 생성 및 부착
	AAttachmentBase* NewAttachmentActor = GetWorld()->SpawnActorDeferred<AAttachmentBase>(AttachmentData->AttachmentClass, FTransform::Identity, GetOwner(), GetOwner()->GetInstigator(), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	NewAttachmentActor->SetAttachmentData(AttachmentData);
	NewAttachmentActor->FinishSpawning(FTransform::Identity);
	
	NewAttachmentActor->SetActorRelativeTransform(AttachmentData->AttachTransform);
	NewAttachmentActor->AttachToComponent(OwnerMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachmentData->AttachSocketName);
	
	EquippedAttachment.AttachmentActor = NewAttachmentActor;
	
	OnAttachmentAdded(AttachmentSlot, EquippedAttachment);
}

void UAttachmentManageComponent::RemoveAttachment_Internal(EAttachmentSlot AttachmentSlot)
{
	FEquippedAttachment Removed;
	if (EquippedAttachments.RemoveAndCopyValue(AttachmentSlot, Removed))
	{
		if (IsValid(Removed.AttachmentActor))
		{
			OnAttachmentRemoved(AttachmentSlot, Removed);
			
			Removed.AttachmentActor->Destroy();
		}
	}
}

void UAttachmentManageComponent::UpdateOwnerMeshBoneVisibility(EAttachmentSlot AttachmentSlot, bool bAttachmentAdded) const
{
	// Bone - SKM only
	USkeletalMeshComponent* OwnerSKM = Cast<USkeletalMeshComponent>(OwnerMeshComponent);
	if (!OwnerSKM)
	{
		return;
	}
	
	// 설정이 없으면 early return
	const auto* Config = BoneVisibilityConfigs.Find(AttachmentSlot);
	if (!Config)
	{
		return;
	}
	
	// 숨김/표시
	for (const FName& BoneName : Config->BonesToHide)
	{
		if (bAttachmentAdded)
		{
			OwnerSKM->HideBoneByName(BoneName, PBO_None);
		}
		else
		{
			OwnerSKM->UnHideBoneByName(BoneName);
		}
	}
			
	// 표시/숨김
	for (const FName& BoneName : Config->BonesToShow)
	{
		if (bAttachmentAdded)
		{
			OwnerSKM->UnHideBoneByName(BoneName);
		}
		else
		{
			OwnerSKM->HideBoneByName(BoneName, PBO_None);
		}
	}
}

#if WITH_EDITOR
#include "Misc/DataValidation.h"

EDataValidationResult UAttachmentManageComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	UEnum* EnumPtr = StaticEnum<EAttachmentSlot>();
	if (!EnumPtr)
	{
		return Result;
	}
	
	const auto MemberName = GET_MEMBER_NAME_STRING_CHECKED(UAttachmentManageComponent, DefaultAttachmentData);
	
	for (const auto& Pair : DefaultAttachmentData)
	{
		const FString SlotName = EnumPtr->GetNameByValue(static_cast<int64>(Pair.Key)).ToString();
		
		if (Pair.Key >= EAttachmentSlot::MAX || !EnumPtr->IsValidEnumName(*SlotName))
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid AttachmentSlot key found in %s for slot %s"), MemberName, *SlotName)));
			Result = EDataValidationResult::Invalid;
		}
		
		if (!Pair.Value.IsValid())
		{
			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid GameplayTag value found in %s for slot %s"), MemberName, *SlotName)));
			Result = EDataValidationResult::Invalid;
		}
	}
	
	return Result;
}
#endif
