// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"

#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/FPPlayerState.h"
#include "System/FPAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPCharacter)

AFPCharacter::AFPCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->GravityScale = 1.f;
	GetCharacterMovement()->MaxAcceleration = 2400.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.f;
	GetCharacterMovement()->BrakingFriction = 6.f;
	GetCharacterMovement()->GroundFriction = 8.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1400.f;
	GetCharacterMovement()->RotationRate.Yaw = 720.f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));

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
}

void AFPCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TargetCameraHeight = BaseEyeHeight * 2.f;
	if (bIsCrouched && IsMovingFromInput())
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
	return GetVelocity().Size2D() > 0.f && GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;
}

void AFPCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize Player Skeletal Mesh Component
	if (USkeletalMesh* SKM = UFPAssetManager::GetAssetById<USkeletalMesh>(TEXT("SKM_Player")))
	{
		GetMesh()->SetSkeletalMesh(SKM);
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

	ApplyAbilitySystemData(TEXT("ASD_Default"));
}

void AFPCharacter::ApplyAbilitySystemData(const FName& DataId)
{
	if (HasAuthority())
	{
		if (const UFPAbilitySystemData* AbilitySystemData = UFPAssetManager::GetAssetById<UFPAbilitySystemData>(DataId))
		{
			AbilitySystemData->GiveDataToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
		}
	}
}
