// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIManageComponent.generated.h"

/**
 * Main Widget을 생성하고 관리하는 Actor Component
 *
 * Owner가 Local Player Controller 일 때 위젯을 생성한다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UUIManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUIManageComponent();

	UUserWidget* GetMainWidget() const { return MainWidget; }

	template <class T>
	T* GetMainWidget() const { return Cast<T>(MainWidget); }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> MainWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> MainWidget;
};
