// Fill out your copyright notice in the Description page of Project Settings.


#include "MoteTitleController.h"
#include "../UI/Title/TitleWidget.h"
#include "../UI/Main/OptionsWidget.h"


AMoteTitleController::AMoteTitleController()
{
	static ConstructorHelpers::FClassFinder<UTitleWidget>
		TitleAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Title.UI_Title_C'"));

	if (TitleAsset.Succeeded())
		mTitleWidgetClass = TitleAsset.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget>
		CursorAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/ETC/UI_Cursor.UI_Cursor_C'"));

	if (CursorAsset.Succeeded())
		mCursorWidgetClass = CursorAsset.Class;

	static ConstructorHelpers::FClassFinder<UOptionsWidget>
		OptionsUIAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Options.UI_Options_C'"));

	if (OptionsUIAsset.Succeeded())
		mOptionsWidgetClass = OptionsUIAsset.Class;
}

void AMoteTitleController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(mTitleWidgetClass))
	{
		mTitleWidget = CreateWidget<UTitleWidget>(GetWorld(), mTitleWidgetClass);

		if (IsValid(mTitleWidget))
			mTitleWidget->AddToViewport();
	}

	if (IsValid(mCursorWidgetClass))
	{
		mCursorWidget = CreateWidget<UUserWidget>(GetWorld(), mCursorWidgetClass);

		if (IsValid(mCursorWidget))
		{
			SetMouseCursorWidget(EMouseCursor::Default, mCursorWidget);
		}

		bShowMouseCursor = true;
	}

	if (IsValid(mOptionsWidgetClass))
	{
		mOptionsWidget = CreateWidget<UOptionsWidget>(GetWorld(), mOptionsWidgetClass);

		if (IsValid(mOptionsWidget))
		{
			mOptionsWidget->AddToViewport();
			mOptionsWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AMoteTitleController::VisibleOptionsWidget(UUserWidget* Widget)
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
