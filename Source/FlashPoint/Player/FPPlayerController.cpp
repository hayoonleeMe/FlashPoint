// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "FPGameplayTags.h"
#include "FPPlayerState.h"
#include "AbilitySystem/FPAbilitySystemComponent.h"
#include "Camera/FPPlayerCameraManager.h"
#include "Character/FPCharacter.h"
#include "Component/UIManageComponent.h"
#include "Data/FPInputData.h"
#include "Game/FPGameState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/FPInputComponent.h"
#include "System/FPAssetManager.h"
#include "UI/Gameplay/MatchResult.h"
#include "UI/Gameplay/PauseMenu.h"
#include "UI/Gameplay/Scoreboard/Scoreboard.h"
#include "Weapon/WeaponManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FPPlayerController)

AFPPlayerController::AFPPlayerController()
{
	PlayerCameraManagerClass = AFPPlayerCameraManager::StaticClass();
}

void AFPPlayerController::SetUIInputMode()
{
	Super::SetUIInputMode();

	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// 추가로, 게임플레이 중 UI Input을 활성화하면 Gameplay IMC 제거
		Subsystem->RemoveMappingContext(InputData->GameplayMappingContext);
	}
	
	// 마우스 커서를 화면 중앙에 설정
	int32 ViewportX, ViewportY;
	GetViewportSize(ViewportX, ViewportY);
	SetMouseLocation(ViewportX * 0.5f, ViewportY * 0.5f);
}

void AFPPlayerController::SetGameplayInputMode()
{
	Super::SetGameplayInputMode();

	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputData->GameplayMappingContext, 0);
		Subsystem->RemoveMappingContext(InputData->UIMappingContext);
	}
}

void AFPPlayerController::SetInitialInputMode()
{
	SetGameplayInputMode();
}

void AFPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const UFPInputData* InputData = UFPAssetManager::GetAssetById<UFPInputData>(TEXT("InputData"));
	check(InputData);

	UFPInputComponent* FPInputComponent = CastChecked<UFPInputComponent>(InputComponent);

	// Bind Native Inputs
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Gameplay::Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Gameplay::Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Gameplay::PauseMenu, ETriggerEvent::Triggered, this, &ThisClass::Input_PauseMenu);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Gameplay::Scoreboard, ETriggerEvent::Triggered, this, &ThisClass::Input_Scoreboard);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Gameplay::QuickSlot::Slot1, ETriggerEvent::Triggered, this, &ThisClass::Input_QuickSlot1);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Gameplay::QuickSlot::Slot2, ETriggerEvent::Triggered, this, &ThisClass::Input_QuickSlot2);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Gameplay::QuickSlot::Slot3, ETriggerEvent::Triggered, this, &ThisClass::Input_QuickSlot3);
	FPInputComponent->BindNativeAction(InputData, FPGameplayTags::Input::Gameplay::ChangeView, ETriggerEvent::Triggered, this, &ThisClass::Input_ChangeView);

	// Bind Ability Inputs
	FPInputComponent->BindAbilityActions(InputData, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);
}

void AFPPlayerController::Input_Move(const FInputActionValue& InputValue)
{
	if (GetPawn())
	{
		const FVector2D MovementVector = InputValue.Get<FVector2D>();
		const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

		if (MovementVector.X != 0.f)
		{
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			GetPawn()->AddMovementInput(ForwardDirection, MovementVector.X);
		}
		if (MovementVector.Y != 0.f)
		{
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			GetPawn()->AddMovementInput(RightDirection, -MovementVector.Y);	
		}
	}
}

void AFPPlayerController::Input_Look(const FInputActionValue& InputValue)
{
	const FVector2D LookAxisVector = InputValue.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(-LookAxisVector.Y);
}

void AFPPlayerController::Input_PauseMenu()
{
	UUserWidget* PauseMenuWidget = UIManageComponent->AddWidget(EWidgetLayer::Menu, PauseMenuClass);
	PauseMenuWidget->OnNativeDestruct.AddLambda([this](UUserWidget* InWidget)
	{
		SetGameplayInputMode();
	});

	SetUIInputMode();
}

void AFPPlayerController::Input_Scoreboard(const FInputActionValue& InputValue)
{
	if (Scoreboard)
	{
		Scoreboard->ShowWidget(InputValue.Get<bool>());
	}
}

void AFPPlayerController::Input_QuickSlot1()
{
	if (UWeaponManageComponent* WeaponManageComponent = UWeaponManageComponent::Get(GetPawn()))
	{
		WeaponManageComponent->ServerEquipWeaponAtSlot(1);
	}
}

void AFPPlayerController::Input_QuickSlot2()
{
	if (UWeaponManageComponent* WeaponManageComponent = UWeaponManageComponent::Get(GetPawn()))
	{
		WeaponManageComponent->ServerEquipWeaponAtSlot(2);
	}
}

void AFPPlayerController::Input_QuickSlot3()
{
	if (UWeaponManageComponent* WeaponManageComponent = UWeaponManageComponent::Get(GetPawn()))
	{
		WeaponManageComponent->ServerEquipWeaponAtSlot(3);
	}
}

void AFPPlayerController::Input_ChangeView()
{
	if (AFPCharacter* FPCharacter = Cast<AFPCharacter>(GetCharacter()))
	{
		FPCharacter->ChangeView();
	}
}

void AFPPlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UFPAbilitySystemComponent* ASC = GetFPAbilitySystemComponent())
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AFPPlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UFPAbilitySystemComponent* ASC = GetFPAbilitySystemComponent())
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

UFPAbilitySystemComponent* AFPPlayerController::GetFPAbilitySystemComponent() const
{
	if (const AFPPlayerState* PS = GetPlayerState<AFPPlayerState>())
	{
		return PS->GetFPAbilitySystemComponent();
	}
	return nullptr;
}

void AFPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 클라이언트 로컬 플레이어 Only
	if (IsLocalController() && GetWorld())
	{
		if (AFPGameState* FPGameState = GetWorld()->GetGameState<AFPGameState>())
		{
			FPGameState->OnClientMatchInfoReplicatedDelegate.AddUObject(this, &ThisClass::OnClientMatchInfoReplicated);
			FPGameState->OnMatchEndedDelegate.AddUObject(this, &ThisClass::OnMatchEnded);
			FPGameState->OnMatchEndTimeDilationFinishedDelegate.AddUObject(this, &ThisClass::OnMatchEndTimeDilationFinished);
		}
	}
}

void AFPPlayerController::OnClientMatchInfoReplicated(const FMatchInfo& MatchInfo)
{
	if (TSubclassOf<UScoreboard>* ScoreboardClass = ScoreboardClasses.Find(MatchInfo.MatchMode))
	{
		// Scoreboard Widget 생성 후 숨김
    	Scoreboard = UIManageComponent->AddWidget<UScoreboard>(EWidgetLayer::HUD, *ScoreboardClass);
    	Scoreboard->ShowWidget(false);
	}
}

void AFPPlayerController::OnMatchEnded()
{
	if (MatchResultClass)
	{
		MatchResult = UIManageComponent->AddWidget<UMatchResult>(EWidgetLayer::Popup, MatchResultClass);
	}
}

void AFPPlayerController::OnMatchEndTimeDilationFinished()
{
	// MatchResult Widget 제거
	if (MatchResult)
	{
		UIManageComponent->RemoveWidget(EWidgetLayer::Popup, MatchResult);
	}
	
	// Scoreboard를 표시
	if (Scoreboard)
	{
		Scoreboard->ShowWidget(true);
	}

	// 입력 제한
	DisableInput(this);

	// 모든 어빌리티 취소
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
	{
		ASC->CancelAllAbilities();
	}
}
