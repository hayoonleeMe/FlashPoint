// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/MatchTypes.h"
#include "PlayerHUD.generated.h"

class UCrosshair;
class UWidgetSwitcher;
class UMatchTimer;
class UMiniScoreboard;
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
	// 클라이언트로 MatchInfo 프로퍼티가 Replicate될 때 호출되는 Callback
	void OnClientMatchInfoReplicated(const FMatchInfo& MatchInfo);

	// MatchMode 별 Mini Scoreboard Widget Class
	UPROPERTY(EditDefaultsOnly)
	TMap<EMatchMode, TSubclassOf<UMiniScoreboard>> MiniScoreboardClasses;

	// 클라이언트로 PlayerState가 Replicate될 때 호출되는 Callback
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

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNamedSlot> NamedSlot_MiniScoreboard;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMatchTimer> MatchTimer;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher_Crosshair;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCrosshair> Crosshair_Default;
	
	UPROPERTY(meta=(BindWidget))
    TObjectPtr<UCrosshair> Crosshair_Shotgun;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCrosshair> Crosshair_Simple;

	// 현재 WidgetSwitcher_Crosshair에서 활성화된 위젯
	UPROPERTY()
	TWeakObjectPtr<UCrosshair> CachedCrosshairWidget;

	// 장착 중인 무기가 변경될 때 호출되는 Callback
	void OnEquippedWeaponChanged(AWeapon_Base* EquippedWeapon);

	// AimSpread가 변경될 때 호출되는 Callback
	void OnAimSpreadChanged(float AimSpread);
};
