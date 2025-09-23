// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FPGameplayTags.h"
#include "AbilitySystem/Attributes/FPAttributeSet.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Player/BasePlayerController.h"
#include "Weapon/WeaponManageComponent.h"
#include "Weapon/Weapon_Base.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerHUD)

void UPlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ABasePlayerController* BasePC = GetOwningPlayer<ABasePlayerController>())
	{
		// PlayerState가 유효해질 때 초기화 로직 수행
		BasePC->OnPlayerStateReplicatedDelegate.AddUObject(this, &ThisClass::OnPlayerStateReplicated);
	}

	WeaponSlotImages = {	Image_WeaponSlot1, Image_WeaponSlot2, Image_WeaponSlot3	};
	for (UImage* Image : WeaponSlotImages)
	{
		Image->SetBrushFromTexture(UnarmedIcon, true);
	}

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		if (UWeaponManageComponent* WeaponManageComponent = OwningPawn->FindComponentByClass<UWeaponManageComponent>())
		{
			// Handle Ammo, ReserveAmmo Change
			WeaponManageComponent->RegisterAmmoTagStackChangedEvent(FPGameplayTags::Weapon::Data::Ammo, FOnAmmoTagStackChangedDelegate::FDelegate::CreateUObject(this, &ThisClass::OnWeaponAmmoChanged));
			WeaponManageComponent->RegisterAllReserveAmmoChangedEvent(FOnAmmoTagStackChangedDelegate::FDelegate::CreateUObject(this, &ThisClass::OnWeaponReserveAmmoChanged));

			// 무기 장착 상태에 따라 HUD 업데이트
			WeaponManageComponent->OnWeaponEquipStateChangedDelegate.AddUObject(this, &ThisClass::OnWeaponEquipStateChanged);
			OnWeaponEquipStateChanged(0, nullptr);
		}
	}
}

void UPlayerHUD::NativePreConstruct()
{
	Super::NativePreConstruct();

	Text_Ammo->SetVisibility(ESlateVisibility::Hidden);
	Text_ReserveAmmo->SetVisibility(ESlateVisibility::Hidden);
	Text_WeaponName->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerHUD::OnPlayerStateReplicated(APlayerState* PlayerState)
{
	if (PlayerState)
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerState))
		{
			// Handle Health, MaxHealth Change
			ASC->GetGameplayAttributeValueChangeDelegate(UFPAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
			ASC->GetGameplayAttributeValueChangeDelegate(UFPAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

			// Initialize Health, MaxHealth
			bool bFound = false;
			Health = ASC->GetGameplayAttributeValue(UFPAttributeSet::GetHealthAttribute(), bFound);
			MaxHealth = ASC->GetGameplayAttributeValue(UFPAttributeSet::GetMaxHealthAttribute(), bFound);
			if (bFound && MaxHealth != 0.f)
			{
				ProgressBar_Health->SetPercent(Health / MaxHealth);
			}
		}
	}
}

void UPlayerHUD::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	Health = Data.NewValue; 
	ProgressBar_Health->SetPercent(Health / MaxHealth);
}

void UPlayerHUD::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	MaxHealth = Data.NewValue; 
	ProgressBar_Health->SetPercent(Health / MaxHealth);
}

void UPlayerHUD::OnWeaponAmmoChanged(const FGameplayTag& Tag, int32 StackCount)
{
	Text_Ammo->SetText(FText::AsNumber(StackCount));
}

void UPlayerHUD::OnWeaponReserveAmmoChanged(const FGameplayTag& Tag, int32 StackCount)
{
	Text_ReserveAmmo->SetText(FText::AsNumber(StackCount));
}

void UPlayerHUD::OnWeaponEquipStateChanged(int32 ActiveSlotIndex, AWeapon_Base* EquippedWeapon)
{
	// 무기 장착 여부에 따라 Visibility 업데이트
	const ESlateVisibility NewVisibility =  EquippedWeapon != nullptr ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	Text_Ammo->SetVisibility(NewVisibility);
	Text_ReserveAmmo->SetVisibility(NewVisibility);
	Text_WeaponName->SetVisibility(NewVisibility);

	// 현재 슬롯에 대한 무기 정보를 HUD에 업데이트
	if (WeaponSlotImages.IsValidIndex(ActiveSlotIndex))
	{
		WeaponSlotImages[ActiveSlotIndex]->SetColorAndOpacity(FLinearColor::White);
		if (EquippedWeapon)
		{
			WeaponSlotImages[ActiveSlotIndex]->SetBrushFromTexture(EquippedWeapon->GetDisplayIcon(), true);
			Text_WeaponName->SetText(FText::FromString(EquippedWeapon->GetDisplayName()));
		}
		else
		{
			WeaponSlotImages[ActiveSlotIndex]->SetBrushFromTexture(UnarmedIcon, true);
		}
	}

	// 현재 슬롯을 제외한 나머지 슬롯의 이미지는 회색으로 표시
	for (int32 Index = 0; Index < WeaponSlotImages.Num(); ++Index)
	{
		if (Index != ActiveSlotIndex)
		{
			WeaponSlotImages[Index]->SetColorAndOpacity(FLinearColor::Gray);
		}
	}
}
