// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIManageComponent.generated.h"

/**
 * 위젯을 표시할 레이어를 나타내는 enum class
 *
 * 표시하는 우선순위가 낮은 레이어부터 오름차순이다.
 */
UENUM(BlueprintType)
enum class EWidgetLayer : uint8
{
	World,
	HUD,
	Menu,
	Popup,
	MAX UMETA(Hidden)
};

/**
 * USTRUCT Wrapper for UUserWidget TArray 
 */
USTRUCT()
struct FActiveWidgetArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<UUserWidget>> Widgets;
};

/**
 * Widget Layer 별로 위젯을 생성하고 관리하는 액터 컴포넌트
 *
 * 또한 UI Input을 연동하고 관리한다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLASHPOINT_API UUIManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static UUIManageComponent* Get(const APlayerController* OwningPC);
	
	UUIManageComponent();
	virtual void InitializeComponent() override;

	// HUD Layer의 메인 위젯을 반환한다.
	UUserWidget* GetMainHUDWidget() const;

	template <class T>
	T* GetMainHUDWidget() const { return Cast<T>(GetMainHUDWidget()); }

	// WidgetLayer 레이어에 WidgetClass 타입 위젯을 생성하고 반환한다.
	UUserWidget* AddWidget(EWidgetLayer WidgetLayer, const TSubclassOf<UUserWidget>& WidgetClass);

	template <class T>
	T* AddWidget(EWidgetLayer WidgetLayer, const TSubclassOf<UUserWidget>& WidgetClass)
	{
		return Cast<T>(AddWidget(WidgetLayer, WidgetClass));
	}

	// WidgetLayer 레이어에 생성된 WidgetClass 타입인 위젯을 모두 제거한다.
	void RemoveWidgets(EWidgetLayer WidgetLayer, const TSubclassOf<UUserWidget>& WidgetClass);

	// WidgetLayer 레이어에 생성된 WidgetToRemove 위젯을 제거한다.
	void RemoveWidget(EWidgetLayer WidgetLayer, UUserWidget* WidgetToRemove);

protected:
	virtual void BeginPlay() override;

private:
	// HUD Layer의 메인 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

	// 위젯 레이어 별로 생성한 위젯들을 관리하는 맵
	// FActiveWidgetArray 내부 배열의 순서는 위젯 생성 순으로 보장된다.
	UPROPERTY()
	TMap<EWidgetLayer, FActiveWidgetArray> ActiveWidgetMap;

	// 우선순위가 가장 높은 최상단 위젯을 반환한다.
	UUserWidget* GetTopWidget() const;

	// ============================================================================
	// Input 
	// ============================================================================
	
	void OnSetupInputComponent(UInputComponent* InputComponent);
	void Input_UI_Back();
	void Input_UI_Confirm();
};
