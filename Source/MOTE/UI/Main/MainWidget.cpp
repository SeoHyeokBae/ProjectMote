// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "NiagaraSystemWidget.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	mSkillStoneImage = Cast<UImage>(GetWidgetFromName(TEXT("SkillStone")));
	mSkillStoneImage->SetVisibility(ESlateVisibility::Collapsed);

	mSkillStoneBack = Cast<UImage>(GetWidgetFromName(TEXT("SkillBack")));
	mSkillStoneBack->SetVisibility(ESlateVisibility::Collapsed);
	mSkillStoneFrame = Cast<UImage>(GetWidgetFromName(TEXT("SkillFrame")));
	mSkillStoneFrame->SetVisibility(ESlateVisibility::Collapsed);

	mPlayerHP = Cast<UUserWidget>(GetWidgetFromName(TEXT("UI_PlayerInfo")));
	mPlayerHP->SetVisibility(ESlateVisibility::Collapsed);

	mNiagaraWidget = Cast<UNiagaraSystemWidget>(GetWidgetFromName(TEXT("NS_Widget")));
	mNiagaraStoneLight = Cast<UNiagaraSystemWidget>(GetWidgetFromName(TEXT("NS_StoneLight")));
}

void UMainWidget::AppearWidget()
{
	PlayAnimation(FadeIn);
}

void UMainWidget::DisappearWidget()
{
	PlayAnimation(FadeOut);
}

void UMainWidget::ShowGuide()
{
	PlayAnimation(FadeInGuide);
}

void UMainWidget::HideGuide()
{
	PlayAnimation(FadeOutGuide);
}

