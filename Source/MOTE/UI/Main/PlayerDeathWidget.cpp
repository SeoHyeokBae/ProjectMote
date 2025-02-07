// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Main/PlayerDeathWidget.h"
#include "Controller/MotePlayerController.h"
#include "GameMode/MainGameMode.h"
#include "../../Sound/SoundSubsystem.h"

void UPlayerDeathWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TitleName = Cast<UUserWidget>(GetWidgetFromName(TEXT("UI_DeadName")));
	ReStartButton = Cast<UButton>(GetWidgetFromName(TEXT("ReStartButton")));
	ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("ExitButton")));
	SelectReStart = Cast<UImage>(GetWidgetFromName(TEXT("SelectReStartImg")));
	SelectExit = Cast<UImage>(GetWidgetFromName(TEXT("SelectExitImg")));
	BackGround = Cast<UImage>(GetWidgetFromName(TEXT("BackGround")));
	Blur = Cast<UBackgroundBlur>(GetWidgetFromName(TEXT("BackGroundBlur")));

	ReStartButton->OnClicked.AddDynamic(this, &UPlayerDeathWidget::ReStartButtonClick);
	ExitButton->OnClicked.AddDynamic(this, &UPlayerDeathWidget::ExitButtonClick);

	ReStartButton->OnHovered.AddDynamic(this, &UPlayerDeathWidget::ReStartButtonHover);
	ExitButton->OnHovered.AddDynamic(this, &UPlayerDeathWidget::ExitButtonHover);

	ReStartButton->OnUnhovered.AddDynamic(this, &UPlayerDeathWidget::ReStartButtonUnhover);
	ExitButton->OnUnhovered.AddDynamic(this, &UPlayerDeathWidget::ExitButtonUnhover);

	TitleName->SetRenderOpacity(0.0f);
	ReStartButton->SetRenderOpacity(0.f);
	ExitButton->SetRenderOpacity(0.f);
	BackGround->SetRenderOpacity(0.f);
	Blur->SetBlurStrength(0.f);

	ReStartButton->SetVisibility(ESlateVisibility::Collapsed);
	ExitButton->SetVisibility(ESlateVisibility::Collapsed);

	UpdateCanTick();
}

void UPlayerDeathWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UPlayerDeathWidget::ReStartButtonClick()
{
	USoundSubsystem* SoundSystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSystem->SetResetVolume();

	FName CurrentLevelName = *GetWorld()->GetMapName();
	CurrentLevelName = FName(CurrentLevelName.ToString().Replace(TEXT("UEDPIE_0_"), TEXT(""))); // PIE 모드에서는 "UEDPIE_0_" 접두사가 붙으므로 제거
	UGameplayStatics::OpenLevel(GetWorld(), CurrentLevelName,true);
}

void UPlayerDeathWidget::ExitButtonClick()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

void UPlayerDeathWidget::ReStartButtonHover()
{
	SelectReStart->SetVisibility(ESlateVisibility::Visible);

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
		SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
	}
}

void UPlayerDeathWidget::ExitButtonHover()
{
	SelectExit->SetVisibility(ESlateVisibility::Visible);

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
		SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
	}
}

void UPlayerDeathWidget::ReStartButtonUnhover()
{
	SelectReStart->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerDeathWidget::ExitButtonUnhover()
{
	SelectExit->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerDeathWidget::AppearWidget()
{
	PlayAnimation(FadeIn);

	ReStartButton->SetVisibility(ESlateVisibility::Visible);
	ExitButton->SetVisibility(ESlateVisibility::Visible);
}
