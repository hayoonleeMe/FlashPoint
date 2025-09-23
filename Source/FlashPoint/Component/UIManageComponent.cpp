// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManageComponent.h"

#include "FPLogChannels.h"
#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIManageComponent)

UUIManageComponent* UUIManageComponent::Get(const APlayerController* OwningPC)
{
	return OwningPC ? OwningPC->FindComponentByClass<UUIManageComponent>() : nullptr;
}

UUserWidget* UUIManageComponent::GetTopWidget() const
{
	for (int32 Index = static_cast<int32>(EWidgetLayer::MAX) - 1; Index >= 0; --Index)
	{
		EWidgetLayer WidgetLayer = static_cast<EWidgetLayer>(Index);
		if (const FActiveWidgetArray* ActiveWidgetArray = ActiveWidgetMap.Find(WidgetLayer))
		{
			if (!ActiveWidgetArray->Widgets.IsEmpty())
			{
				return ActiveWidgetArray->Widgets.Last();
			}
		}
	}
	return nullptr;
}

UUserWidget* UUIManageComponent::GetMainHUDWidget() const
{
	if (const FActiveWidgetArray* Array = ActiveWidgetMap.Find(EWidgetLayer::HUD))
	{
		for (UUserWidget* Widget : Array->Widgets)
		{
			if (Widget->IsA(MainHUDWidgetClass))
			{
				return Widget;
			}
		}
	}
	return nullptr;
}

UUserWidget* UUIManageComponent::AddWidget(EWidgetLayer WidgetLayer, const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		if (PC->IsLocalController())
		{
			if (WidgetLayer != EWidgetLayer::MAX && WidgetClass)
			{
				if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, WidgetClass))
				{
					Widget->AddToViewport(static_cast<int32>(WidgetLayer));

					FActiveWidgetArray& ActiveWidgetStack = ActiveWidgetMap.FindOrAdd(WidgetLayer);
					ActiveWidgetStack.Widgets.Add(Widget);
							
					return Widget;
				}
			}
			else
			{
				UE_LOG(LogFP, Error, TEXT("[%hs] Invalid WidgetLayer or WidgetClass."), __FUNCTION__);
			}
		}
	}
	return nullptr;
}

void UUIManageComponent::RemoveWidgets(EWidgetLayer WidgetLayer, const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (WidgetLayer != EWidgetLayer::MAX && WidgetClass)
	{
		if (FActiveWidgetArray* ActiveWidgetArray = ActiveWidgetMap.Find(WidgetLayer))
		{
			// WidgetClass 타입인지 체크
			auto Predicate = [&WidgetClass](const UUserWidget* Target) -> bool
			{
				return Target && Target->IsA(WidgetClass);
			};

			// 우선 화면에서 제거
			for (UUserWidget* Widget : ActiveWidgetArray->Widgets)
			{
				if (Predicate(Widget))
				{
					Widget->RemoveFromParent();
				}
			}

			// 배열에서 제거
			ActiveWidgetArray->Widgets.RemoveAll(Predicate);
		}
	}
	else
	{
		UE_LOG(LogFP, Error, TEXT("[%hs] Invalid WidgetLayer or WidgetClass."), __FUNCTION__);
	}
}

void UUIManageComponent::RemoveWidget(EWidgetLayer WidgetLayer, UUserWidget* WidgetToRemove)
{
	if (WidgetLayer != EWidgetLayer::MAX && WidgetToRemove)
	{
		if (FActiveWidgetArray* ActiveWidgetArray = ActiveWidgetMap.Find(WidgetLayer))
		{
			if (!ActiveWidgetArray->Widgets.IsEmpty())
			{
				WidgetToRemove->RemoveFromParent();
				if (WidgetToRemove == ActiveWidgetArray->Widgets.Last())
				{
					ActiveWidgetArray->Widgets.Pop();
				}
				else
				{
					ActiveWidgetArray->Widgets.Remove(WidgetToRemove);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogFP, Error, TEXT("[%hs] Invalid WidgetLayer or WidgetToRemove."), __FUNCTION__);
	}
}

void UUIManageComponent::BeginPlay()
{
	Super::BeginPlay();

	// UI
	AddWidget(EWidgetLayer::HUD, MainHUDWidgetClass);
}
