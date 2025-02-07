// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSkillWidget.h"

void UPlayerSkillWidget::NativeConstruct()
{
	mSkillFrame = Cast<UImage>(GetWidgetFromName(TEXT("SkillFrame")));
	mSkillBack = Cast<UImage>(GetWidgetFromName(TEXT("SkillBack")));
	mSkillMeteor = Cast<UImage>(GetWidgetFromName(TEXT("SkillMeteor")));
	
	if(!mGetMeteor)
		mSkillMeteor->SetVisibility(ESlateVisibility::Collapsed);
	else
		mSkillMeteor->SetVisibility(ESlateVisibility::Visible);
}
