// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SendGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_SendGameplayEvent)

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (EventTag.IsValid() && MeshComp)
	{
		if (AActor* Owner = MeshComp->GetOwner())
		{
			const UWorld* World = Owner->GetWorld();
			// 이 Notify를 추가한 Anim Montage 애셋을 열 때 ASC를 가지고 있지 않은 Animation Editor Preview Actor에 의해 Error Log가 출력되는 것을 방지하기 위해 GameWorld인지 체크
			if (World && World->IsGameWorld())
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, FGameplayEventData());
			}
		}
	}
}

