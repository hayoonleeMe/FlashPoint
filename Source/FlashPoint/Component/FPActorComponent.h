// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPActorComponent.generated.h"

namespace FPUtils
{
	// OwnerActor의 T 타입 컴포넌트를 반환하는 헬퍼 함수
	template <class T>
	FORCEINLINE static T* GetComponent(const AActor* OwnerActor)
	{
		if (IsValid(OwnerActor))
		{
			return OwnerActor->FindComponentByClass<T>();
		}
		return nullptr;
	}	
}

/**
 * 커스텀 ActorComponent를 구현할 때 유용한 기능을 구현하는 Abstract Class
 */
UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UFPActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool HasAuthority() const;
	bool IsPlayerLocallyControlled() const;
};
