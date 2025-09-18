// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/MatchTypes.h"
#include "GameFramework/PlayerStart.h"
#include "TeamPlayerStart.generated.h"

/**
 * 팀 별로 플레이어를 소환하는 PlayerStart
 */
UCLASS(PrioritizeCategories="Team")
class FLASHPOINT_API ATeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	ATeamPlayerStart(const FObjectInitializer& ObjectInitializer);

	ETeam GetTeam() const { return Team; }

private:
	UPROPERTY(EditAnywhere, Category="Team")
	ETeam Team;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PadMeshComponent;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> TeamColorMID;

	void SetPadMeshColorByTeam();

	static constexpr FLinearColor FFAColor = FLinearColor(0.666667f, 0.587570f, 0.203389f);

#if WITH_EDITOR
public:
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
};
