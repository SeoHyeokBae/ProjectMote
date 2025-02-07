// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/TargetAimSystemComponent.h"
#include "Components/WidgetComponent.h"

UTargetAimSystemComponent::UTargetAimSystemComponent()
{		
	// TargetAimSystem
	UPrimitiveComponent::SetCollisionProfileName(TEXT("Target"));
	mLockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	//mLockOnWidget->SetupAttachment(this);
	mLockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
	// LockOnWidget
	static ConstructorHelpers::FClassFinder<UUserWidget> BP_LockOnWidget(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Monster/UI_LockOnWidget.UI_LockOnWidget_C'"));
	if (BP_LockOnWidget.Succeeded())
	{
		mLockOnWidget->SetWidgetClass(BP_LockOnWidget.Class);
	}
	mLockOnWidget->SetDrawSize(FVector2D(25.0f, 25.0f));
	mLockOnWidget->SetHiddenInGame(true);
}

void UTargetAimSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
