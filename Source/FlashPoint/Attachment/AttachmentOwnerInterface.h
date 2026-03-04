// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AttachmentOwnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAttachmentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * UAttachmentManageComponent를 사용하는 Owner가 구현해야하는 인터페이스
 */
class FLASHPOINT_API IAttachmentOwnerInterface
{
	GENERATED_BODY()

public:
	// 부착물이 장착될 Owner의 메시 컴포넌트를 반환한다.
	virtual UMeshComponent* GetAttachmentOwnerMeshComponent() const = 0;
};
