// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SendGameplayEvent.generated.h"

/**
 * Notify를 받을 때 Owner에 Gameplay Event를 전송하는 Anim Notify 
 */
UCLASS()
class FLASHPOINT_API UAnimNotify_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(EditAnywhere, Category=AnimNotify)
	FGameplayTag EventTag;
};
