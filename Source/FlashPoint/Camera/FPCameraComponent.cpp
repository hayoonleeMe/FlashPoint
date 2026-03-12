// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCameraComponent)

void FCameraOffsetView::Blend(const FCameraOffsetView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.f)
	{
		return;
	}
	if (OtherWeight >= 1.f)
	{
		*this = Other;
		return;
	}
	
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);
	
	Rotation = FMath::Lerp(Rotation, Other.Rotation, OtherWeight);
	// const FRotator DeltaAng = (Other.Rotation - Rotation).GetNormalized();
	// Rotation = Rotation + OtherWeight * DeltaAng;
	
	FOV = FMath::Lerp(FOV, Other.FOV, OtherWeight);
}

UFPCameraOffset::UFPCameraOffset()
{
	BlendAlpha = 1.f;
	BlendWeight = 1.f;
}

void UFPCameraOffset::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.f, 1.f);
	
	// todo : blend function, blend exponent로 역산해 Alpha 계산
	BlendAlpha = BlendWeight;
}

void UFPCameraOffset::UpdateCameraOffset(float DeltaTime, FCameraOffsetView& OutOffsetView)
{
	UpdateOffsetView(DeltaTime);
	UpdateBlending(DeltaTime);
	OutOffsetView = OffsetView;
}

void UFPCameraOffset::UpdateOffsetView(float DeltaTime)
{
	OffsetView.Location = OffsetLocation;
	OffsetView.Rotation = OffsetRotation;
	OffsetView.FOV = OffsetFOV;
}

void UFPCameraOffset::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0.f)
	{
		BlendAlpha += DeltaTime / BlendTime;
		BlendAlpha = FMath::Min(BlendAlpha, 1.f);
	}
	else
	{
		BlendAlpha = 1.f;
	}
	
	// todo : blend function, blend exponent로 Weight 계산
	BlendWeight = BlendAlpha;
}

UFPCameraComponent::UFPCameraComponent()
{
}

void UFPCameraComponent::Deactivate()
{
	Super::Deactivate();
	
	MainCameraOffset = nullptr;
	PendingCameraOffset = nullptr;
	SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
}

void UFPCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	DetermineCameraOffsets();
	
	FCameraOffsetView FinalOffsetView;
	UpdateCameraOffsets(DeltaTime, FinalOffsetView);
	
	// Offset
	SetRelativeLocationAndRotation(FinalOffsetView.Location, FinalOffsetView.Rotation);

	Super::GetCameraView(DeltaTime, DesiredView);
	
	DesiredView.FOV += FinalOffsetView.FOV;
}

void UFPCameraComponent::DetermineCameraOffsets()
{
	if (!IsActive())
	{
		return;
	}
	
	if (DetermineCameraOffsetDelegate.IsBound())
	{
		TSubclassOf<UFPCameraOffset> CameraOffsetClass = DetermineCameraOffsetDelegate.Execute();
		if (!CameraOffsetClass)
		{
			CameraOffsetClass = UFPCameraOffset::StaticClass();
		}
		
		// 오프셋 오브젝트가 이미 Main 오프셋이면 무시
		if (MainCameraOffset && CameraOffsetClass == MainCameraOffset->GetClass())
		{
			return;
		}

		// 인스턴스
		UFPCameraOffset* CameraOffset = GetCameraOffsetInstance(CameraOffsetClass);
		check(CameraOffset);

		const bool bShouldBlend = CameraOffset->GetBlendTime() > 0.f;
		float BlendWeight = bShouldBlend ? 0.f : 1.f;
		if (bShouldBlend && MainCameraOffset && CameraOffset == PendingCameraOffset)
		{
			// CameraOffset이 Pending이라면 웨이트를 이어받는다.
			// Pending은 Main의 웨이트를 제외한만큼 적용되므로 그만큼을 이어받는다.
			BlendWeight = 1.f - MainCameraOffset->GetBlendWeight();
		}
		CameraOffset->SetBlendWeight(BlendWeight);
		
		PendingCameraOffset = MainCameraOffset;
		MainCameraOffset = CameraOffset;
	}
}

void UFPCameraComponent::UpdateCameraOffsets(float DeltaTime, FCameraOffsetView& OutOffsetView)
{
	if (!IsActive())
	{
		return;
	}

	if (PendingCameraOffset)
	{
		// Pending 오프셋을 베이스로 계산
		OutOffsetView.Blend(PendingCameraOffset->GetOffsetView(), PendingCameraOffset->GetBlendWeight());
	}
	
	if (MainCameraOffset)
	{
		// Main 오프셋 블렌드
		FCameraOffsetView MainOffsetView;
		MainCameraOffset->UpdateCameraOffset(DeltaTime, MainOffsetView);
		
		const float MainBlendWeight = MainCameraOffset->GetBlendWeight();
		OutOffsetView.Blend(MainOffsetView, MainBlendWeight);

		if (MainBlendWeight >= 1.f)
		{
			// Main 블렌드가 끝나면 Pending 제거
			PendingCameraOffset = nullptr;
		}
	}
}

UFPCameraOffset* UFPCameraComponent::GetCameraOffsetInstance(TSubclassOf<UFPCameraOffset> CameraOffsetClass)
{
	check(CameraOffsetClass);
	
	// 이미 캐싱되어 있으면 바로 반환
	for (UFPCameraOffset* Instance : CachedCameraOffsetInstances)
	{
		if (Instance && Instance->GetClass() == CameraOffsetClass)
		{
			return Instance;
		}
	}
	
	// 캐싱 후 반환
	UFPCameraOffset* NewInstance = NewObject<UFPCameraOffset>(this, CameraOffsetClass);
	CachedCameraOffsetInstances.Add(NewInstance);
	return NewInstance;
}
