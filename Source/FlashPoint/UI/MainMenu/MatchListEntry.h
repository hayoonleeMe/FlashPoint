// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "MatchListEntry.generated.h"

class UBorder;
class UTextBlock;

/**
 * UMatchListPage의 ListView에 표시할 Entry Widget
 */
UCLASS()
class FLASHPOINT_API UMatchListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void HighlightEntry(bool bHighlight) const;

protected:
	// Begin IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
	// End IUserObjectListEntry
	
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_RoomName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_MatchMode;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border;

	static constexpr FLinearColor NormalColor = FLinearColor(0.f, 0.f, 0.f, 0.6f);
	static constexpr FLinearColor SelectedColor = FLinearColor(0.34375f, 0.34375f, 0.328805f, 0.5f);
};
