// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"

#include "FlashPoint.h"
#include "FPCharacterMovementComponent.h"
#include "FPGameplayTags.h"
#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/WeaponWallTraceComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/FPAssetData.h"
#include "Data/FPCosmeticData.h"
#include "Weapon/WeaponManageComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/FPPlayerState.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCharacter)

AFPCharacter::AFPCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFPCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->MaxWalkSpeed = 360.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 250.f;
	GetCharacterMovement()->GravityScale = 1.2f;
	GetCharacterMovement()->MaxAcceleration = 2400.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.f;
	GetCharacterMovement()->BrakingFriction = 6.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1400.f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->RotationRate.Yaw = 720.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.f);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Spawner, ECR_Overlap);
	GetCapsuleComponent()->SetCapsuleRadius(48.f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Block);

	BaseEyeHeight = 80.f;
	CrouchedEyeHeight = 50.f;
	CurrentCameraHeight = TargetCameraHeight = BaseEyeHeight * 2.f;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->SetRelativeLocation(FVector(-35.f, 0.f, CurrentCameraHeight));
	SpringArmComponent->SocketOffset = FVector(0.f, 35.f, 0.f);
	SpringArmComponent->TargetArmLength = 350.f;
	SpringArmComponent->bDoCollisionTest = true;
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPS Camera Component"));
	FPSCameraComponent->SetupAttachment(GetMesh(), TEXT("FPSCamera"));
	FPSCameraComponent->bUsePawnControlRotation = true;
	FPSCameraComponent->bAutoActivate = false;

	WeaponManageComponent = CreateDefaultSubobject<UWeaponManageComponent>(TEXT("Weapon Manage Component"));
	
	WeaponWallTraceComponent = CreateDefaultSubobject<UWeaponWallTraceComponent>(TEXT("Weapon Wall Trace Component"));
}

void AFPCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const bool bIsMovingFromInput = IsMovingFromInput();
	if (AbilitySystemComponent && bIsMovingFromInput != bWasMovingFromInput)
	{
		// 캐릭터 움직임 여부에 따라 IsMoving 태그 업데이트
		if (bIsMovingFromInput)
		{
			AbilitySystemComponent->AddLooseGameplayTag(FPGameplayTags::CharacterState::IsMoving);
		}
		else
		{
			AbilitySystemComponent->RemoveLooseGameplayTag(FPGameplayTags::CharacterState::IsMoving);
		}
	}
	bWasMovingFromInput = bIsMovingFromInput;

	TargetCameraHeight = BaseEyeHeight * 2.f;
	if (bIsCrouched && bWasMovingFromInput)
	{
		// Crouch + Walk 하면 캐릭터가 살짝 일어나서 걸으므로 카메라 위치도 높여준다.
		TargetCameraHeight += 20.f;
	}
	CurrentCameraHeight = FMath::FInterpTo(CurrentCameraHeight, TargetCameraHeight, DeltaSeconds, 7.f);
	const FVector CameraLocation = SpringArmComponent->GetRelativeLocation();
	SpringArmComponent->SetRelativeLocation(FVector(CameraLocation.X, CameraLocation.Y, CurrentCameraHeight));
}

void AFPCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystem();
}

void AFPCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilitySystem();
	SetCharacterMesh();
}

UAbilitySystemComponent* AFPCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAbilitySystemComponent* AFPCharacter::GetFPAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFPAttributeSet* AFPCharacter::GetFPAttributeSet() const
{
	return AttributeSet;
}

bool AFPCharacter::IsMovingFromInput() const
{
	return GetVelocity().SizeSquared2D() > 0.f && GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;
}

bool AFPCharacter::CanJumpInternal_Implementation() const
{
	// Crouch 체크 무시
	return JumpIsAllowedInternal();
}

void AFPCharacter::ChangeView() const
{
	if (AbilitySystemComponent)
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(FPGameplayTags::CharacterState::FPS))
		{
			// to TPS
			AbilitySystemComponent->RemoveLooseGameplayTag(FPGameplayTags::CharacterState::FPS);
			FPSCameraComponent->Deactivate();
			CameraComponent->Activate();
		}
		else
		{
			// to FPS
			AbilitySystemComponent->AddLooseGameplayTag(FPGameplayTags::CharacterState::FPS);
			CameraComponent->Deactivate();
			FPSCameraComponent->Activate();
		}
	}
}

void AFPCharacter::SetCharacterMesh()
{
	UFPCosmeticData* CosmeticData = UFPAssetManager::GetAssetById<UFPCosmeticData>(TEXT("CosmeticData"));
	check(CosmeticData);

	// Initialize Player Skeletal Mesh Component
	if (USkeletalMesh* SKM = CosmeticData->GetCharacterMesh())
	{
		GetMesh()->SetSkeletalMesh(SKM);
	}

	// Set Materials By Team
	if (ABasePlayerState* BasePS = GetPlayerState<ABasePlayerState>())
	{
		if (BasePS->GetTeam() != ETeam::None)
		{
			if (const FCharacterMeshEntry* Entry = CosmeticData->GetCharacterMeshEntryByTeam(BasePS->GetTeam()))
			{
				const TArray<TObjectPtr<UMaterialInterface>>& Materials = Entry->Materials;
				for (int32 Index = 0; Index < Materials.Num(); ++Index)
				{
					GetMesh()->SetMaterial(Index, Materials[Index]);	
				}
			}
		}
	}
	
	// Anim Layer
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->LinkAnimClassLayers(CosmeticData->GetDefaultAnimLayer());
	}
}

void AFPCharacter::InitAbilitySystem()
{
	if (AFPPlayerState* PS = GetPlayerState<AFPPlayerState>())
	{
		AbilitySystemComponent = PS->GetFPAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		AttributeSet = PS->GetFPAttributeSet();
	}

	if (HasAuthority())
	{
		ApplyAbilitySystemData(FPGameplayTags::Asset::AbilitySystemData::Default);
	}
}

void AFPCharacter::ApplyAbilitySystemData(const FGameplayTag& DataTag)
{
	check(HasAuthority());
	
	if (const UFPAbilitySystemData* AbilitySystemData = UFPAssetManager::GetAssetByTag<UFPAbilitySystemData>(DataTag))
	{
		AbilitySystemData->GiveDataToAbilitySystem(AbilitySystemComponent);
	}
}

void AFPCharacter::RemoveAbilitySystemData(const FGameplayTag& DataTag)
{
	check(HasAuthority());
	
	if (const UFPAbilitySystemData* AbilitySystemData = UFPAssetManager::GetAssetByTag<UFPAbilitySystemData>(DataTag))
	{
		AbilitySystemData->RemoveDataFromAbilitySystem(AbilitySystemComponent);
	}
}
