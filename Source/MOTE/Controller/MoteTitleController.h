// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/PlayerController.h"
#include "MoteTitleController.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API AMoteTitleController : public APlayerController
{
	GENERATED_BODY()

public:
	AMoteTitleController();

protected:
	virtual void BeginPlay() override;
	
protected:
	TSubclassOf<class UTitleWidget>	mTitleWidgetClass;
	class UTitleWidget* mTitleWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> mCursorWidgetClass;

	UPROPERTY(VisibleAnywhere)
	class UUserWidget* mCursorWidget;

	TSubclassOf<class UOptionsWidget>	mOptionsWidgetClass;
	class UOptionsWidget* mOptionsWidget;

public:
	void VisibleOptionsWidget(UUserWidget* Widget);
};
