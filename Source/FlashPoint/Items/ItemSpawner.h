// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

class USphereComponent;

USTRUCT(BlueprintType)
struct FItemSpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ItemClass;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditDefaultsOnly)
	FTransform ItemMeshTransform;

	UPROPERTY(EditDefaultsOnly)
	float SphereRadius = 0.f;
};

/**
 * 무기, 탄약 등의 아이템을 소환하는 액터
 */
UCLASS()
class FLASHPOINT_API AItemSpawner : public AActor
{
	GENERATED_BODY()

public:
	AItemSpawner();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

protected:
	// Only called on server
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PadMeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ItemMeshComponent;

	// 소환할 아이템들
	UPROPERTY(EditDefaultsOnly, Category="FlashPoint")
	TArray<FItemSpawnInfo> ItemInfos;

	// 아이템 회전 속도
	UPROPERTY(EditDefaultsOnly, Category="FlashPoint")
	float MeshRotateSpeed;

	// 랜덤한 아이템을 선택할 시간
	UPROPERTY(EditDefaultsOnly, Category="FlashPoint")
	float ItemChangeDelay;

	FTimerHandle ItemChangeTimerHandle;

	void ChangeItem();

	// 현재 선택된 아이템의 ItemInfos 배열 인덱스
	UPROPERTY(ReplicatedUsing=OnRep_ActiveItemIndex)
	int32 ActiveItemIndex;

	UFUNCTION()
	void OnRep_ActiveItemIndex();
};
