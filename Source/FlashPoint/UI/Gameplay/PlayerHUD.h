// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class AWeapon_Base;
struct FGameplayTag;
struct FOnAttributeChangeData;
class UImage;
class UTextBlock;
class UProgressBar;

/**
 * 게임플레이 정보를 화면에 표시하는 HUD
 */
UCLASS()
class FLASHPOINT_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

private:
	void OnPlayerStateReplicated(APlayerState* PlayerState);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Health;

	// Owning Character's Health Attribute Value
	float Health = 0.f;
	void OnHealthChanged(const FOnAttributeChangeData& Data);

	// Owning Character's MaxHealth Attribute Value
	float MaxHealth = 0.f;
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Ammo;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ReserveAmmo;

	// Ammo가 변경될 때 호출되는 Callback
	void OnWeaponAmmoChanged(const FGameplayTag& Tag, int32 StackCount);

	// Reserve Ammo가 변경될 때 호출되는 Callback
	void OnWeaponReserveAmmoChanged(const FGameplayTag& Tag, int32 StackCount);

	// 무기 장착 상태가 변경될 때 호출되는 Callback
	// 무기 관련 정보를 HUD에 업데이트한다.
	void OnWeaponEquipStateChanged(int32 ActiveSlotIndex, AWeapon_Base* EquippedWeapon);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_WeaponName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_WeaponSlot1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_WeaponSlot2;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_WeaponSlot3;

	UPROPERTY()
	TArray<UImage*> WeaponSlotImages;

	// 무기를 장착하지 않은 슬롯에 표시할 아이콘
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> UnarmedIcon;
};
