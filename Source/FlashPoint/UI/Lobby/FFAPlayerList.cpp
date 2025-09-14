// Fill out your copyright notice in the Description page of Project Settings.


#include "FFAPlayerList.h"

#include "PlayerListRow.h"
#include "Components/VerticalBox.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FFAPlayerList)

void UFFAPlayerList::OnPlayerJoined(const FString& Username) const
{
	const int32 NumLeft = VerticalBox_Left->GetChildrenCount();
	const int32 NumRight = VerticalBox_Right->GetChildrenCount();

	UVerticalBox* BoxToAdd = NumLeft <= NumRight ? VerticalBox_Left : VerticalBox_Right;
	if (UPlayerListRow* PlayerListRow = CreateWidget<UPlayerListRow>(BoxToAdd, PlayerListRowClass))
	{
		PlayerListRow->SetUsername(Username);
		if (Username == PlayerUsername)
		{
			// 플레이어 이름 강조 표시
			PlayerListRow->HighlightUsername();
		}
		
		BoxToAdd->AddChildToVerticalBox(PlayerListRow);
	}
}

void UFFAPlayerList::OnPlayerLeft(const FString& Username) const
{
	// LeftBox에서 찾아서 제거
	for (UWidget* Widget : VerticalBox_Left->GetAllChildren())
	{
		if (UPlayerListRow* PlayerListRow = Cast<UPlayerListRow>(Widget))
		{
			if (PlayerListRow->GetUsername() == Username)
			{
				VerticalBox_Left->RemoveChild(PlayerListRow);
				return;
			}
		}
	}

	// RightBox에서 찾아서 제거
	for (UWidget* Widget : VerticalBox_Right->GetAllChildren())
	{
		if (UPlayerListRow* PlayerListRow = Cast<UPlayerListRow>(Widget))
		{
			if (PlayerListRow->GetUsername() == Username)
			{
				VerticalBox_Right->RemoveChild(PlayerListRow);
				return;
			}
		}
	}
}
