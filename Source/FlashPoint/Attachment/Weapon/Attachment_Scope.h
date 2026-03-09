// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttachmentBase.h"
#include "Attachment_Scope.generated.h"

class UScopeLensMaskComponent;

/**
 * 스코프 부착물을 나타내는 액터 클래스
 */
UCLASS()
class FLASHPOINT_API AAttachment_Scope : public AWeaponAttachmentBase
{
	GENERATED_BODY()

public:
	AAttachment_Scope();
	virtual void OnConstruction(const FTransform& Transform) override;
	
	// Begin IWeaponAttachmentInterface
	virtual void OnWeaponEquipped() override;
	virtual void OnWeaponUnEquipped() override;
	virtual void StartAimDownSight() override;
	virtual void StopAimDownSight() override;
	// End IWeaponAttachmentInterface

protected:
	virtual void BeginPlay() override;
	
	// AimDownSight 시, Primary Mesh 대신 화면에 표시될 축소된 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Attachment")
	TObjectPtr<UStaticMeshComponent> ADSMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Attachment")
	FName ADSMeshAttachSocketName;
	
	// AimDownSight 시, 스코프 렌즈를 표시할 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Attachment")
	TObjectPtr<UStaticMeshComponent> LensMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Attachment")
	FName LensMeshAttachSocketName;

	// ============================================================================
	// ScopeLensMask
	// ============================================================================
	
	UPROPERTY(VisibleAnywhere, Category="ScopeLensMask")
	TObjectPtr<UScopeLensMaskComponent> ScopeLensMaskComponent;
};
