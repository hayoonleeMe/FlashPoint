// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamPlayerStart.h"

#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamPlayerStart)

ATeamPlayerStart::ATeamPlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Team = ETeam::None;

	PadMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pad Mesh Component"));
	PadMeshComponent->SetupAttachment(GetRootComponent());
	PadMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	PadMeshComponent->SetRelativeScale3D(FVector(0.8f));
}

void ATeamPlayerStart::SetPadMeshColorByTeam()
{
	if (!TeamColorMID)
	{
		TeamColorMID = PadMeshComponent->CreateDynamicMaterialInstance(0);
	}
	
	if (TeamColorMID)
	{
		switch (Team)
		{
		case ETeam::RedTeam:
			TeamColorMID->SetVectorParameterValue(TEXT("Color"), FLinearColor::Red);
			break;
			
		case ETeam::BlueTeam:
			TeamColorMID->SetVectorParameterValue(TEXT("Color"), FLinearColor::Blue);
			break;
			
		default: // FFA
			TeamColorMID->SetVectorParameterValue(TEXT("Color"), FFAColor);
			break;
		}
	}
}

#if WITH_EDITOR
void ATeamPlayerStart::OnConstruction(const FTransform& Transform)
{
	SetPadMeshColorByTeam();
}
#endif
