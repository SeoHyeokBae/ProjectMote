// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Main/OptionsWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Animation/WidgetAnimation.h"
#include "../../Controller/MotePlayerController.h"
#include "../../Sound/SoundSubsystem.h"
#include "GameFramework/PlayerController.h"

void UOptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AcceptButton = Cast<UButton>(GetWidgetFromName(TEXT("BackButton")));
	MasterMuteBotton = Cast<UButton>(GetWidgetFromName(TEXT("Master_Mute_Off")));
	BGMMuteBotton = Cast<UButton>(GetWidgetFromName(TEXT("BGM_Mute_Off")));
	VFXMuteBotton = Cast<UButton>(GetWidgetFromName(TEXT("VFX_Mute_Off")));

	MasterSlider = Cast<USlider>(GetWidgetFromName(TEXT("Masterslide")));
	BGMSlider = Cast<USlider>(GetWidgetFromName(TEXT("BGMslide")));
	VFXSlider = Cast<USlider>(GetWidgetFromName(TEXT("VFXslide")));

	AcceptButton->OnClicked.AddDynamic(this, &UOptionsWidget::AcceptButtonClick);
	AcceptButton->OnHovered.AddDynamic(this, &UOptionsWidget::AcceptButtonHover);
	AcceptButton->OnUnhovered.AddDynamic(this, &UOptionsWidget::AcceptButtonUnhover);

	MasterMuteBotton->OnClicked.AddDynamic(this, &UOptionsWidget::MasterMuteClick);
	MasterMuteBotton->OnHovered.AddDynamic(this, &UOptionsWidget::MasterMuteHover);
	MasterMuteBotton->OnUnhovered.AddDynamic(this, &UOptionsWidget::MasterMuteUnhover);

	BGMMuteBotton->OnClicked.AddDynamic(this, &UOptionsWidget::BGMMuteClick);
	BGMMuteBotton->OnHovered.AddDynamic(this, &UOptionsWidget::BGMMuteHover);
	BGMMuteBotton->OnUnhovered.AddDynamic(this, &UOptionsWidget::BGMMuteUnhover);

	VFXMuteBotton->OnClicked.AddDynamic(this, &UOptionsWidget::VFXMuteClick);
	VFXMuteBotton->OnHovered.AddDynamic(this, &UOptionsWidget::VFXMuteHover);
	VFXMuteBotton->OnUnhovered.AddDynamic(this, &UOptionsWidget::VFXMuteUnhover);

	MasterSlider->OnValueChanged.AddDynamic(this, &UOptionsWidget::MasterValueChange);
	MasterSlider->OnMouseCaptureBegin.AddDynamic(this, &UOptionsWidget::MasterSlideCaputeBegin);
	MasterSlider->OnMouseCaptureEnd.AddDynamic(this, &UOptionsWidget::MasterSlideCaputeEnd);

	BGMSlider->OnValueChanged.AddDynamic(this, &UOptionsWidget::BGMValueChange);
	BGMSlider->OnMouseCaptureBegin.AddDynamic(this, &UOptionsWidget::BGMSlideCaputeBegin);
	BGMSlider->OnMouseCaptureEnd.AddDynamic(this, &UOptionsWidget::BGMSlideCaputeEnd);

	VFXSlider->OnValueChanged.AddDynamic(this, &UOptionsWidget::VFXValueChange);
	VFXSlider->OnMouseCaptureBegin.AddDynamic(this, &UOptionsWidget::VFXSlideCaputeBegin);
	VFXSlider->OnMouseCaptureEnd.AddDynamic(this, &UOptionsWidget::VFXSlideCaputeEnd);

	SoundSystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
}

void UOptionsWidget::UpdateSlide(float MasterVolume, float BGMVolume, float VFXVolume)
{
	MasterSlider->SetValue(MasterVolume);
	BGMSlider->SetValue(BGMVolume);
	VFXSlider->SetValue(VFXVolume);
}

void UOptionsWidget::AcceptButtonClick()
{
	APlayerController* Controller = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (Controller && prevWidget)
	{
		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(prevWidget->TakeWidget());
		Controller->SetInputMode(InputModeData);
		SetVisibility(ESlateVisibility::Collapsed);
		prevWidget->SetVisibility(ESlateVisibility::Visible);
		if (SoundSystem)
		{
			SoundSystem->PlayVFXSound(TEXT("SelectButton"));
		}
	}
}

void UOptionsWidget::AcceptButtonHover()
{
	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (SoundSubsystem)
	{
		SoundSubsystem->PlayVFXSound(TEXT("UIBottonHover"));
	}

	if (AcceptHoverAnim)
	{
		PlayAnimation(AcceptHoverAnim, 0, 0, EUMGSequencePlayMode:: PingPong);
	}
}

void UOptionsWidget::AcceptButtonUnhover()
{
	if (AcceptHoverAnim)
	{
		StopAnimation(AcceptHoverAnim);
		AcceptButton->SetColorAndOpacity(FLinearColor(0.5, 0.5, 0.5));
	}
}

void UOptionsWidget::MasterMuteClick()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("SelectButton"));
		if (IsMasterMute)
		{
			IsMasterMute = false;
			SoundSystem->SetMasterVolume(PrevMasterVolume);
			StopAnimation(MasterHoverAnim);
			TempColor = FLinearColor(1.f, 1.f, 1.f);
			MasterMuteBotton->SetColorAndOpacity(TempColor);
		}
		else
		{
			IsMasterMute = true;
			PrevMasterVolume = SoundSystem->GetMasterVolume();
			SoundSystem->SetMasterVolume(0);
			StopAnimation(MasterHoverAnim);
			TempColor = FLinearColor(1.f, 0.f, 0.f);
			MasterMuteBotton->SetColorAndOpacity(TempColor);
		}
	}
}

void UOptionsWidget::MasterMuteHover()
{
	if (MasterHoverAnim)
	{
		TempColor = MasterMuteBotton->GetColorAndOpacity();
		PlayAnimation(MasterHoverAnim, 0, 0, EUMGSequencePlayMode::PingPong);
	}
}

void UOptionsWidget::MasterMuteUnhover()
{
	if (MasterHoverAnim)
	{
		StopAnimation(MasterHoverAnim);
		MasterMuteBotton->SetColorAndOpacity(TempColor);
	}
}

void UOptionsWidget::BGMMuteClick()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("SelectButton"));
		if (IsBGMMute)
		{
			IsBGMMute = false;
			SoundSystem->SetBGMVolume(PrevBGMVolume);
			StopAnimation(BGMHoverAnim);
			TempColor = FLinearColor(1.f, 1.f, 1.f);
			BGMMuteBotton->SetColorAndOpacity(TempColor);
		}
		else
		{
			IsBGMMute = true;
			PrevBGMVolume = SoundSystem->GetBGMVolume();
			SoundSystem->SetBGMVolume(0);
			StopAnimation(BGMHoverAnim);
			TempColor = FLinearColor(1.f, 0.f, 0.f);
			BGMMuteBotton->SetColorAndOpacity(TempColor);
		}
	}
}

void UOptionsWidget::BGMMuteHover()
{
	if (BGMHoverAnim)
	{
		TempColor = BGMMuteBotton->GetColorAndOpacity();
		PlayAnimation(BGMHoverAnim, 0, 0, EUMGSequencePlayMode::PingPong);
	}
}

void UOptionsWidget::BGMMuteUnhover()
{
	if (BGMHoverAnim)
	{
		StopAnimation(BGMHoverAnim);
		BGMMuteBotton->SetColorAndOpacity(TempColor);
	}
}

void UOptionsWidget::VFXMuteClick()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("SelectButton"));
		if (IsVFXMute)
		{
			IsVFXMute = false;
			SoundSystem->SetVFXVolume(PrevVFXVolume);
			StopAnimation(VFXHoverAnim);
			TempColor = FLinearColor(1.f, 1.f, 1.f);
			VFXMuteBotton->SetColorAndOpacity(TempColor);
		}
		else
		{
			IsVFXMute = true;
			PrevVFXVolume = SoundSystem->GetVFXVolume();
			SoundSystem->SetVFXVolume(0);
			StopAnimation(VFXHoverAnim);
			TempColor = FLinearColor(1.f, 0.f, 0.f);
			VFXMuteBotton->SetColorAndOpacity(TempColor);
		}
	}
}

void UOptionsWidget::VFXMuteHover()
{
	if (VFXHoverAnim)
	{
		TempColor = VFXMuteBotton->GetColorAndOpacity();
		PlayAnimation(VFXHoverAnim, 0, 0, EUMGSequencePlayMode::PingPong);
	}
}

void UOptionsWidget::VFXMuteUnhover()
{
	if (VFXHoverAnim)
	{
		StopAnimation(VFXHoverAnim);
		VFXMuteBotton->SetColorAndOpacity(TempColor);
	}
}

void UOptionsWidget::MasterValueChange(float InValue)
{
	if (SoundSystem)
	{
		if (IsVisible())
			SoundSystem->PlayVFXSound(TEXT("Slider"));
		SoundSystem->SetMasterVolume(InValue);
		if (InValue < 0.01f && IsMasterMute == false)
		{
			IsMasterMute = true;
			MasterMuteBotton->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f));
		}
		else if (InValue > 0.f && IsMasterMute == true)
		{
			IsMasterMute = false;
			MasterMuteBotton->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f));
		}
	}
}

void UOptionsWidget::MasterSlideCaputeBegin()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("Slider"));
	}
}

void UOptionsWidget::MasterSlideCaputeEnd()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("SelectButton"));
	}
}

void UOptionsWidget::BGMValueChange(float InValue)
{
	if (SoundSystem)
	{
		if (IsVisible())
			SoundSystem->PlayVFXSound(TEXT("Slider"));
		SoundSystem->SetBGMVolume(InValue);
		if (InValue < 0.01f && IsBGMMute == false)
		{
			IsBGMMute = true;
			BGMMuteBotton->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f));
		}
		else if (InValue > 0.f && IsBGMMute == true)
		{
			IsBGMMute = false;
			BGMMuteBotton->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f));
		}
	}
}

void UOptionsWidget::BGMSlideCaputeBegin()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("Slider"));
	}
}

void UOptionsWidget::BGMSlideCaputeEnd()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("SelectButton"));
	}
}

void UOptionsWidget::VFXValueChange(float InValue)
{
	if (SoundSystem)
	{
		if(IsVisible())
			SoundSystem->PlayVFXSound(TEXT("Slider"));
		SoundSystem->SetVFXVolume(InValue);
		if (InValue < 0.01f && IsVFXMute == false)
		{
			IsVFXMute = true;
			VFXMuteBotton->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f));
		}
		else if (InValue > 0.f && IsVFXMute == true)
		{
			IsVFXMute = false;
			VFXMuteBotton->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f));
		}
	}
}

void UOptionsWidget::VFXSlideCaputeBegin()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("Slider"));
	}
}

void UOptionsWidget::VFXSlideCaputeEnd()
{
	if (SoundSystem)
	{
		SoundSystem->PlayVFXSound(TEXT("SelectButton"));
	}
}
