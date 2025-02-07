// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Main/PlayerPauseWidget.h"
#include "../../Sound/SoundSubsystem.h"
#include "../../Controller/MotePlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"

void UPlayerPauseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("ResumeButton")));
    OptionButton = Cast<UButton>(GetWidgetFromName(TEXT("OptionButton")));
    QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("QuitButton")));
    SelectResume = Cast<UImage>(GetWidgetFromName(TEXT("SelectResumeImg")));
    SelectOption = Cast<UImage>(GetWidgetFromName(TEXT("SelectOptionsImg")));
    SelectQuit = Cast<UImage>(GetWidgetFromName(TEXT("SelectQuitImg")));

    ResumeButton->OnClicked.AddDynamic(this, &UPlayerPauseWidget::ResumeButtonClick);
    OptionButton->OnClicked.AddDynamic(this, &UPlayerPauseWidget::OptionButtonClick);
    QuitButton->OnClicked.AddDynamic(this, &UPlayerPauseWidget::QuitButtonClick);

    ResumeButton->OnHovered.AddDynamic(this, &UPlayerPauseWidget::ResumeButtonHover);
    OptionButton->OnHovered.AddDynamic(this, &UPlayerPauseWidget::OptionButtonHover);
    QuitButton->OnHovered.AddDynamic(this, &UPlayerPauseWidget::QuitButtonHover);

    ResumeButton->OnUnhovered.AddDynamic(this, &UPlayerPauseWidget::ResumeButtonUnhover);
    OptionButton->OnUnhovered.AddDynamic(this, &UPlayerPauseWidget::OptionButtonUnhover);
    QuitButton->OnUnhovered.AddDynamic(this, &UPlayerPauseWidget::QuitButtonUnhover);
}

FReply UPlayerPauseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::P)
    {
        USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
        if (SoundSubsystem)
        {
            SoundSubsystem->ResumeBGM(true);
            SetVisibility(ESlateVisibility::Collapsed);
            AMotePlayerController* Controller = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
            if (Controller)
            {
                Controller->GetWorldSettings()->SetTimeDilation(1.f);
                Controller->InVisiblePauseWidget();
            }

        }

        // 키 입력 처리 완료
        return FReply::Handled();
    }

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UPlayerPauseWidget::ResumeButtonClick()
{
    USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
    if (SoundSubsystem)
    {
        SoundSubsystem->PlayVFXSound(TEXT("SelectButton"));

        SoundSubsystem->SetPauseBGM(false);
        SoundSubsystem->ResumeBGM(true);
        SetVisibility(ESlateVisibility::Collapsed);
        AMotePlayerController* Controller = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        if (Controller)
        {
            Controller->GetWorldSettings()->SetTimeDilation(1.f);
            Controller->InVisiblePauseWidget();
        }
    }
}

void UPlayerPauseWidget::OptionButtonClick()
{
    AMotePlayerController* PlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PlayerController)
    {
        PlayerController->VisibleOptionsWidget(this);
        SetVisibility(ESlateVisibility::Hidden);
        USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
        if (SoundSubsystem)
        {
            SoundSubsystem->PlayVFXSound(TEXT("SelectButton"));
        }
    }
}

void UPlayerPauseWidget::QuitButtonClick()
{
    UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true);
}

void UPlayerPauseWidget::ResumeButtonHover()
{
    SelectResume->SetVisibility(ESlateVisibility::Visible);
    USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
    if (SoundSubsystem)
    {
        SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
    }
}

void UPlayerPauseWidget::OptionButtonHover()
{
    SelectOption->SetVisibility(ESlateVisibility::Visible);
    USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
    if (SoundSubsystem)
    {
        SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
    }
}

void UPlayerPauseWidget::QuitButtonHover()
{
    SelectQuit->SetVisibility(ESlateVisibility::Visible);
    USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
    if (SoundSubsystem)
    {
        SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
    }
}

void UPlayerPauseWidget::ResumeButtonUnhover()
{
    SelectResume->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerPauseWidget::OptionButtonUnhover()
{
    SelectOption->SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayerPauseWidget::QuitButtonUnhover()
{
    SelectQuit->SetVisibility(ESlateVisibility::Collapsed);
}
