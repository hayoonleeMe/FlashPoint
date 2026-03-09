// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttachmentBase.generated.h"

class UAbilitySystemComponent;
class UFPAttachmentData;

/**
 * 부착물을 나타내는 기본 액터 클래스
 */
UCLASS(Abstract)
class FLASHPOINT_API AAttachmentBase : public AActor
{
	GENERATED_BODY()

public:
	AAttachmentBase();
	
	void SetAttachmentData(UFPAttachmentData* InData) { AttachmentData = InData; }
	
	// 부착물 메시에서 SocketName에 해당하는 소켓 트랜스폼을 반환한다.
	virtual FTransform GetAttachmentSocketTransform(const FName& SocketName, ERelativeTransformSpace TransformSpace) const;

protected:
	// 부착물을 구성하는 최상단 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Attachment")
	TObjectPtr<UStaticMeshComponent> PrimaryMesh;
	
	// 부착물 관련 데이터를 저장하는 데이터 애셋
	// 해당 액터 생성 시 주입
	UPROPERTY()
	TObjectPtr<UFPAttachmentData> AttachmentData;
	
	bool IsPlayerLocallyControlled() const;
	
	UAbilitySystemComponent* GetASC() const;
};
