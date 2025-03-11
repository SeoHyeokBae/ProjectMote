// Fill out your copyright notice in the Description page of Project Settings.


#include "MotePlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../UI/Main/MainWidget.h"
#include "../UI/Main/PlayerSkillWidget.h"
#include "UI/Main/PlayerDeathWidget.h"
#include "UI/Main/PlayerInfoWidget.h"
#include "UI/Main/PlayerClearWidget.h"
#include "UI/Main/PlayerPauseWidget.h"
#include "UI/Main/MonsterInfoWidget.h"
#include "UI/Main/OptionsWidget.h"
#include "../Sound/SoundSubsystem.h"


AMotePlayerController::AMotePlayerController()
{
	static ConstructorHelpers::FClassFinder<UMainWidget>
		UIAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Main.UI_Main_C'"));

	if (UIAsset.Succeeded())
		mMainWidgetClass = UIAsset.Class;

	static ConstructorHelpers::FClassFinder<UPlayerSkillWidget>
		SkillUIAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Player/UI_PlayerSkill.UI_PlayerSkill_C'"));

	if (SkillUIAsset.Succeeded())
		mPlayerSkillWidgetClass = SkillUIAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget>
		CursorAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Player/UI_AimCursor.UI_AimCursor_C'"));

	if (CursorAsset.Succeeded())
		mCursorWidgetClass = CursorAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget>
		ArrowCursorAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/ETC/UI_Cursor.UI_Cursor_C'"));

	if (ArrowCursorAsset.Succeeded())
		mArrowCursorWidgetClass = ArrowCursorAsset.Class;

	static ConstructorHelpers::FClassFinder<UPlayerDeathWidget>
		DeathUIAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Dead.UI_Dead_C'"));

	if (DeathUIAsset.Succeeded())
		mDeathWidgetClass = DeathUIAsset.Class;

	static ConstructorHelpers::FClassFinder<UPlayerClearWidget>
		ClearUIAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Clear.UI_Clear_C'"));

	if (ClearUIAsset.Succeeded())
		mClearWidgetClass = ClearUIAsset.Class;

	static ConstructorHelpers::FClassFinder<UPlayerPauseWidget>
		PauseUIAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Pause.UI_Pause_C'"));

	if (PauseUIAsset.Succeeded())
		mPauseWidgetClass = PauseUIAsset.Class;

	static ConstructorHelpers::FClassFinder<UOptionsWidget>
		OptionsUIAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Options.UI_Options_C'"));

	if (OptionsUIAsset.Succeeded())
		mOptionsWidgetClass = OptionsUIAsset.Class;
}

void AMotePlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(mMainWidgetClass))
	{
		mMainWidget = CreateWidget<UMainWidget>(GetWorld(), mMainWidgetClass);

		if (IsValid(mMainWidget))
			mMainWidget->AddToViewport();

	}

	if (IsValid(mPauseWidgetClass))
	{
		mPauseWidget = CreateWidget<UPlayerPauseWidget>(GetWorld(), mPauseWidgetClass);

		if (IsValid(mPauseWidget))
		{
			mPauseWidget->AddToViewport();
			mPauseWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

	}

	if (IsValid(mPlayerSkillWidgetClass))
	{
		mPlayerSkillWidget = CreateWidget<UPlayerSkillWidget>(GetWorld(), mPlayerSkillWidgetClass);
	}

	if (IsValid(mCursorWidgetClass))
	{
		mCursorWidget = CreateWidget<UUserWidget>(GetWorld(), mCursorWidgetClass);

		if (IsValid(mCursorWidget))
		{
			SetMouseCursorWidget(EMouseCursor::Default, mCursorWidget);
		}
	}

	if (IsValid(mDeathWidgetClass))
	{
		mDeathWidget = CreateWidget<UPlayerDeathWidget>(GetWorld(), mDeathWidgetClass);

		if (IsValid(mDeathWidget))
		{
			mDeathWidget->AddToViewport();
			mDeathWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (IsValid(mClearWidgetClass))
	{
		mClearWidget = CreateWidget<UPlayerClearWidget>(GetWorld(), mClearWidgetClass);

		if (IsValid(mClearWidget))
		{
			mClearWidget->AddToViewport();
			mClearWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (IsValid(mOptionsWidgetClass))
	{
		mOptionsWidget = CreateWidget<UOptionsWidget>(GetWorld(), mOptionsWidgetClass);

		if (IsValid(mOptionsWidget))
		{
			mOptionsWidget->AddToViewport();
			mOptionsWidget->SetVisibility(ESlateVisibility::Collapsed);
			USoundSubsystem* SoundSystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
			mOptionsWidget->UpdateSlide(
				SoundSystem->GetMasterVolume(), 
				SoundSystem->GetBGMVolume(), 
				SoundSystem->GetVFXVolume());
		}
	}
}

void AMotePlayerController::UpdateRotation(float DeltaTime)
{
	FVector GravityDirection = FVector::DownVector;
	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
		{
			GravityDirection = MoveComp->GetGravityDirection();
		}
	}

	FRotator ViewRotation = GetControlRotation();

	if (!LastFrameGravity.Equals(FVector::ZeroVector))
	{
		const FQuat DeltaGravityRotation = FQuat::FindBetweenNormals(LastFrameGravity, GravityDirection);
		const FQuat WarpedCameraRotation = DeltaGravityRotation * FQuat(ViewRotation);

		ViewRotation = WarpedCameraRotation.Rotator();
	}
	LastFrameGravity = GravityDirection;

	ViewRotation = GetGravityRelativeRotation(ViewRotation, GravityDirection);

	FRotator DeltaRot(RotationInput);

	if (PlayerCameraManager)
	{
		ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn());

		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);

		ViewRotation.Roll = 0;

		SetControlRotation(GetGravityWorldRotation(ViewRotation, GravityDirection));
	}

	APawn* const P = GetPawnOrSpectator();
	if (P)
	{
		P->FaceRotation(ViewRotation, DeltaTime);
	}
}

FRotator AMotePlayerController::GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		FQuat GravityRotation = FQuat::FindBetweenNormals(GravityDirection, FVector::DownVector);
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}

FRotator AMotePlayerController::GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		FQuat GravityRotation = FQuat::FindBetweenNormals(FVector::DownVector, GravityDirection);
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}

void AMotePlayerController::InVisibleMainWidget()
{
	if (mMainWidget)
	{
		mMainWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMotePlayerController::InVisiblePauseWidget()
{
	if (mPauseWidget)
	{
		mPauseWidget->SetVisibility(ESlateVisibility::Collapsed);
		FInputModeGameOnly InputModeData;
		SetInputMode(InputModeData);

		bShowMouseCursor = false;
	}
}

void AMotePlayerController::VisibleDeathWidget()
{
	if (mMainWidget)
	{
		UUserWidget* PlayerInfoWidget = Cast<UUserWidget>(mMainWidget->GetWidgetFromName(TEXT("UI_PlayerInfo")));
		UPlayerInfoWidget* PlayerInfo = Cast<UPlayerInfoWidget>(PlayerInfoWidget);
		PlayerInfo->ClearHandle();

		UUserWidget* MonsterInfoWidget = Cast<UUserWidget>(mMainWidget->GetWidgetFromName(TEXT("UI_MonsterInfo")));
		UMonsterInfoWidget* MonsterInfo = Cast<UMonsterInfoWidget>(MonsterInfoWidget);
		MonsterInfo->ClearHandle();
	}

	if (mDeathWidget)
	{
		mDeathWidget->SetVisibility(ESlateVisibility::Visible);
		mDeathWidget->AppearWidget();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				GetGameInstance()->GetSubsystem<USoundSubsystem>()->StopBGM(3.5f);
				USoundSubsystem* BGM = GetGameInstance()->GetSubsystem<USoundSubsystem>();
				if (BGM)
				{
					BGM->PlayBGM(TEXT("DeathBGM"), 0.75f, 1.0f);
				}
			}, 1.0f, false);
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (IsValid(mArrowCursorWidgetClass))
			{
				mArrowCursorWidget = CreateWidget<UUserWidget>(GetWorld(), mArrowCursorWidgetClass);

				if (IsValid(mArrowCursorWidget))
				{
					SetMouseCursorWidget(EMouseCursor::Default, mArrowCursorWidget);
					bShowMouseCursor = true;
				}

			}
		}, 2.25f, false);
}

void AMotePlayerController::VisiblePauseWidget()
{
	if (mPauseWidget)
	{
		mPauseWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(mPauseWidget->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputModeData);

		if (IsValid(mArrowCursorWidgetClass))
		{
			mArrowCursorWidget = CreateWidget<UUserWidget>(GetWorld(), mArrowCursorWidgetClass);

			if (IsValid(mArrowCursorWidget))
			{
				SetMouseCursorWidget(EMouseCursor::Default, mArrowCursorWidget);
				bShowMouseCursor = true;
			}

		}
	}
}

void AMotePlayerController::VisibleClearWidget()
{
	if (mClearWidget)
	{
		mClearWidget->SetVisibility(ESlateVisibility::Visible);
		mClearWidget->AppearWidget();

		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(mClearWidget->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputModeData);
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (IsValid(mArrowCursorWidgetClass))
			{
				mArrowCursorWidget = CreateWidget<UUserWidget>(GetWorld(), mArrowCursorWidgetClass);

				if (IsValid(mArrowCursorWidget))
				{
					SetMouseCursorWidget(EMouseCursor::Default, mArrowCursorWidget);
					bShowMouseCursor = true;
				}
			}
		}, 2.25f, false);
}

void AMotePlayerController::VisibleOptionsWidget(UUserWidget* Widget)
{
	if (mOptionsWidget)
	{
		mOptionsWidget->SetPrevWidget(Widget);
		mOptionsWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(mOptionsWidget->TakeWidget());
		SetInputMode(InputModeData);
	}
}
