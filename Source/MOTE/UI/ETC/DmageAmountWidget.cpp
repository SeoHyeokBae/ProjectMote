// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ETC/DmageAmountWidget.h"

void UDmageAmountWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FText UDmageAmountWidget::GetDynamicText()
{
	return FText::AsNumber(DmgAmt);
}

void UDmageAmountWidget::FadeText()
{
	PlayAnimation(Fade);
}

void UDmageAmountWidget::FadeHeadShot()
{
	PlayAnimation(FadeHeadShotText);
}

void UDmageAmountWidget::ChangeColor(FLinearColor NewColor)
{
	if (DmgText)
		DmgText->SetColorAndOpacity(FSlateColor(NewColor));
}

void UDmageAmountWidget::ChangeScale(float DmgAmount)
{
	float NewFontSize = FMath::Clamp(25 * (DmgAmount / 10.0f), 25, 75);
	FSlateFontInfo FontInfo = DmgText->Font;
	FontInfo.Size = NewFontSize;
	DmgText->Font = FontInfo;
}