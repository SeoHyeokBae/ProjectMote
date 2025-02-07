// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "Controller/MoteTitleController.h"
#include "GameMode/TitleGameMode.h"
#include "Player/PlayerTitlePawn.h"
#include "../../Sound/SoundSubsystem.h"


void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	TitleName = Cast<UUserWidget>(GetWidgetFromName(TEXT("UITitleName")));
	StartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartButton")));
	OptionButton = Cast<UButton>(GetWidgetFromName(TEXT("OptionButton")));
	ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("ExitButton")));
	SelectStart = Cast<UImage>(GetWidgetFromName(TEXT("SelectStartImg")));
	SelectOption = Cast<UImage>(GetWidgetFromName(TEXT("SelectOptionImg")));
	SelectExit = Cast<UImage>(GetWidgetFromName(TEXT("SelectExitImg")));

	StartButton->OnClicked.AddDynamic(this, &UTitleWidget::StartButtonClick);
	OptionButton->OnClicked.AddDynamic(this, &UTitleWidget::OptionButtonClick);
	ExitButton->OnClicked.AddDynamic(this, &UTitleWidget::ExitButtonClick);

	StartButton->OnHovered.AddDynamic(this, &UTitleWidget::StartButtonHover);
	OptionButton->OnHovered.AddDynamic(this, &UTitleWidget::OptionButtonHover);
	ExitButton->OnHovered.AddDynamic(this, &UTitleWidget::ExitButtonHover);

	StartButton->OnUnhovered.AddDynamic(this, &UTitleWidget::StartButtonUnhover);
	OptionButton->OnUnhovered.AddDynamic(this, &UTitleWidget::OptionButtonUnhover);
	ExitButton->OnUnhovered.AddDynamic(this, &UTitleWidget::ExitButtonUnhover);
	
	UpdateCanTick();
}

void UTitleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CanGo)
	{
		AMoteTitleController* PlayerController = Cast<AMoteTitleController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PlayerController)
		{
			APlayerTitlePawn* Pawn = Cast<APlayerTitlePawn>(PlayerController->GetPawn());
			if (Pawn)
				Pawn->GetMovementComponent()->Velocity = FVector(300.0, 0.0, 0.0);
		}
	}
	
	if(NextLevel)
	{
		AMoteTitleController* PlayerController = Cast<AMoteTitleController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PlayerController)
		{
			APlayerTitlePawn* Pawn = Cast<APlayerTitlePawn>(PlayerController->GetPawn());
			if (Pawn->GetMovementComponent()->Velocity.Length() < 50.f)
				UGameplayStatics::OpenLevel(GetWorld(), TEXT("Wormhole"));
		}
	}
}

void UTitleWidget::StartButtonClick()
{
	AMoteTitleController* PlayerController = Cast<AMoteTitleController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = false;
		FInputModeGameOnly InputModeData;
		PlayerController->SetInputMode(InputModeData);

		APlayerTitlePawn* Pawn = Cast<APlayerTitlePawn>(PlayerController->GetPawn());
		if (Pawn)
		{
			StartButton->SetVisibility(ESlateVisibility::Collapsed);
			OptionButton->SetVisibility(ESlateVisibility::Collapsed);
			ExitButton->SetVisibility(ESlateVisibility::Collapsed);
			TitleName->SetVisibility(ESlateVisibility::Collapsed);
			
			PlayerController->SetViewTargetWithBlend(Pawn->GetCamera(),2.f ,EViewTargetBlendFunction::VTBlend_Cubic);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTitleWidget::LaunchPawn, 2.5f, false);
			
			

			CanGo = true;

			USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
			if (SoundSubsystem)
			{
				SoundSubsystem->PlayVFXSound(TEXT("SelectButton"));
				SoundSubsystem->StopBGM(7.f);
			}
		}
	}

}

void UTitleWidget::OptionButtonClick()
{
	AMoteTitleController* PlayerController = Cast<AMoteTitleController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->VisibleOptionsWidget(this);

		USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
		if (SoundSubsystem)
		{
			SoundSubsystem->PlayVFXSound(TEXT("SelectButton"));
		}
	}
}

void UTitleWidget::ExitButtonClick()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

void UTitleWidget::StartButtonHover()
{
	SelectStart->SetVisibility(ESlateVisibility::Visible);

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
		SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
	}
}

void UTitleWidget::OptionButtonHover()
{
	SelectOption->SetVisibility(ESlateVisibility::Visible);

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
		SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
	}
}

void UTitleWidget::ExitButtonHover()
{
	SelectExit->SetVisibility(ESlateVisibility::Visible);
	
	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
		SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
	}
}

void UTitleWidget::StartButtonUnhover()
{
	SelectStart->SetVisibility(ESlateVisibility::Collapsed);
}

void UTitleWidget::OptionButtonUnhover()
{
	SelectOption->SetVisibility(ESlateVisibility::Collapsed);
}

void UTitleWidget::ExitButtonUnhover()
{
	SelectExit->SetVisibility(ESlateVisibility::Collapsed);
}

void UTitleWidget::LaunchPawn()
{
	AMoteTitleController* PlayerController = Cast<AMoteTitleController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		APlayerTitlePawn* Pawn = Cast<APlayerTitlePawn>(PlayerController->GetPawn());
		if (Pawn)
		{
			CanGo = false;
			NextLevel = true;
			Pawn->GetMovementComponent()->Velocity = FVector(20000.0, 0.0, 0.0);
			Pawn->EffectOn();

			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, this, &UTitleWidget::AttachCamera, 1.f, false);
		}
	}
}

void UTitleWidget::AttachCamera()
{
	AMoteTitleController* PlayerController = Cast<AMoteTitleController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	APlayerTitlePawn* Pawn = Cast<APlayerTitlePawn>(PlayerController->GetPawn());
	if (PlayerController && Pawn)
	{
		Pawn->GetCamera()->AttachToComponent(Pawn->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("root"));
		Pawn->GetCamera()->SetActorLocation(FVector(15500, 0, 250));
	}
}

