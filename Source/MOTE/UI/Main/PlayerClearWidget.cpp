// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Main/PlayerClearWidget.h"
#include "Controller/MotePlayerController.h"
#include "GameMode/MainGameMode.h"
#include "../../Sound/SoundSubsystem.h"

void UPlayerClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TitleName = Cast<UTextBlock>(GetWidgetFromName(TEXT("Text")));
	ReStartButton = Cast<UButton>(GetWidgetFromName(TEXT("ReStartButton")));
	ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("ExitButton")));
	SelectReStart = Cast<UImage>(GetWidgetFromName(TEXT("SelectReStartImg")));
	SelectExit = Cast<UImage>(GetWidgetFromName(TEXT("SelectExitImg")));
	BackGround = Cast<UImage>(GetWidgetFromName(TEXT("BackGround")));
	Blur = Cast<UBackgroundBlur>(GetWidgetFromName(TEXT("BackGroundBlur")));

	ReStartButton->OnClicked.AddDynamic(this, &UPlayerClearWidget::ReStartButtonClick);
	ExitButton->OnClicked.AddDynamic(this, &UPlayerClearWidget::ExitButtonClick);

	ReStartButton->OnHovered.AddDynamic(this, &UPlayerClearWidget::ReStartButtonHover);
	ExitButton->OnHovered.AddDynamic(this, &UPlayerClearWidget::ExitButtonHover);

	ReStartButton->OnUnhovered.AddDynamic(this, &UPlayerClearWidget::ReStartButtonUnhover);
	ExitButton->OnUnhovered.AddDynamic(this, &UPlayerClearWidget::ExitButtonUnhover);

	TitleName->SetRenderOpacity(0.0f);
	ReStartButton->SetRenderOpacity(0.f);
	ExitButton->SetRenderOpacity(0.f);
	BackGround->SetRenderOpacity(0.f);
	Blur->SetBlurStrength(0.f);

	ReStartButton->SetVisibility(ESlateVisibility::Collapsed);
	ExitButton->SetVisibility(ESlateVisibility::Collapsed);

	UpdateCanTick();
}

void UPlayerClearWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UPlayerClearWidget::ReStartButtonClick()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// 입력 모드: 게임 전용
		FInputModeGameOnly InputModeData;
		PlayerController->SetInputMode(InputModeData);

		// 마우스 커서 숨기기
		PlayerController->bShowMouseCursor = false;
	}

	FName CurrentLevelName = *GetWorld()->GetMapName();
	CurrentLevelName = FName(CurrentLevelName.ToString().Replace(TEXT("UEDPIE_0_"), TEXT(""))); // PIE 모드에서는 "UEDPIE_0_" 접두사가 붙으므로 제거
	UGameplayStatics::OpenLevel(GetWorld(), CurrentLevelName, true);
}

void UPlayerClearWidget::ExitButtonClick()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

void UPlayerClearWidget::ReStartButtonHover()
{
	SelectReStart->SetVisibility(ESlateVisibility::Visible);

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
		SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
	}
}

void UPlayerClearWidget::ExitButtonHover()
{
	SelectExit->SetVisibility(ESlateVisibility::Visible);

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
		SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
	}
}

void UPlayerClearWidget::ReStartButtonUnhover()
{
	SelectReStart->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerClearWidget::ExitButtonUnhover()
{
	SelectExit->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerClearWidget::AppearWidget()
{
	PlayAnimation(FadeIn);

	ReStartButton->SetVisibility(ESlateVisibility::Visible);
	ExitButton->SetVisibility(ESlateVisibility::Visible);
}