// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/OnlineServiceTypes.h"
#include "MatchListPage.generated.h"

class UTextBlock;
class UMatchListObject;
class UButton;
class UListView;

/**
 * 생성된 매치 리스트를 표시하는 Page Widget
 *
 * 매치를 선택해 Join 할 수 있다.
 */
UCLASS()
class FLASHPOINT_API UMatchListPage : public UUserWidget
{
	GENERATED_BODY()

	friend class UMainMenuWidget;

public:
	void InitializeWidget();
	
protected:
	virtual void NativeOnInitialized() override;

private:
	void OnDescribeGameSessionsSucceeded(const TArray<FGameSessionInfo>& GameSessionInfos);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListView> ListView;

	void OnMatchListItemClicked(UObject* Item);

	UPROPERTY()
	TWeakObjectPtr<UMatchListObject> SelectedMatchList;

	void UnselectMatchList();
	void SelectMatchList(UMatchListObject* Item);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Join;

	UFUNCTION()
	void OnJoinButtonClicked();

	void OnJoinSucceeded(const FString& URL);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Back;

	UFUNCTION()
	void OnBackButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Refresh;

	UFUNCTION()
	void OnRefreshButtonClicked();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_StatusMessage;

	void UpdateRefreshStatusMessage(const FString& Message, bool bShouldResetWidget);

	void UpdateJoinStatusMessage(const FString& Message, bool bShouldResetWidget);
};

