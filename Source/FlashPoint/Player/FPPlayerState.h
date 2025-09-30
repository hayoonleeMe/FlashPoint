// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BasePlayerState.h"
#include "FPPlayerState.generated.h"

class UFPAttributeSet;
class UFPAbilitySystemComponent;

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

private:
	// 매치에서 적을 죽인 횟수 
	UPROPERTY(Replicated, VisibleAnywhere, Category="Match Stats")
	int32 KillCount;

	// 매치에서 죽은 횟수
	UPROPERTY(Replicated, VisibleAnywhere, Category="Match Stats")
	int32 DeathCount;
};
