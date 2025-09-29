// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BasePlayerState.h"
#include "FPPlayerState.generated.h"

class UFPAttributeSet;
class UFPAbilitySystemComponent;

// Kill Count가 변경될 때 브로드캐스트하는 델레게이트
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlayerKillCountChangedDelegate, ETeam/*Team*/, int32/*AddAmount*/);

/**
 * 게임플레이에서 사용할 PlayerState
 */
UCLASS()
class FLASHPOINT_API AFPPlayerState : public ABasePlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFPPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFPAbilitySystemComponent* GetFPAbilitySystemComponent() const;
	UFPAttributeSet* GetFPAttributeSet() const;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFPAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFPAttributeSet> AttributeSet;

	// ============================================================================
	// Match Stats
	// ============================================================================
public:
	void AddToKillCount(int32 AddAmount);
	int32 GetKillCount() const { return KillCount; }

	void AddToDeathCount(int32 AddAmount);
	int32 GetDeathCount() const { return DeathCount; }

	virtual FPlayerInfo MakePlayerInfo() const override;

	FOnPlayerKillCountChangedDelegate OnPlayerKillCountChangedDelegate;
	
private:
	// 매치에서 적을 죽인 횟수 
	UPROPERTY(ReplicatedUsing=OnRep_KillCount, VisibleAnywhere, Category="Match Stats")
	int32 KillCount;

	UFUNCTION()
	void OnRep_KillCount(int32 OldKillCount);

	// 매치에서 죽은 횟수
	UPROPERTY(Replicated, VisibleAnywhere, Category="Match Stats")
	int32 DeathCount;
};
