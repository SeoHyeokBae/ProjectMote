// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UIInfo.h"
#include "../../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct();

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
	UPROPERTY(VisibleAnywhere)
	class UUserWidget* TitleName;

	UPROPERTY(VisibleAnywhere)
	class UButton* StartButton;

	UPROPERTY(VisibleAnywhere)
	class UButton* OptionButton;

	UPROPERTY(VisibleAnywhere)
	class UButton* ExitButton;

	UPROPERTY(VisibleAnywhere)
	UImage* SelectStart;

	UPROPERTY(VisibleAnywhere)
	UImage* SelectOption;

	UPROPERTY(VisibleAnywhere)
	UImage* SelectExit;

private:
	FTimerHandle TimerHandle;
	FTimerHandle TimerHandle2;

	bool CanGo = false;
	bool NextLevel = false;

private:
	UFUNCTION()
	void StartButtonClick();

	UFUNCTION()
	void OptionButtonClick();

	UFUNCTION()
	void ExitButtonClick();

	UFUNCTION()
	void StartButtonHover();

	UFUNCTION()
	void OptionButtonHover();

	UFUNCTION()
	void ExitButtonHover();

	UFUNCTION()
	void StartButtonUnhover();

	UFUNCTION()
	void OptionButtonUnhover();

	UFUNCTION()
	void ExitButtonUnhover();

private:
	void LaunchPawn();
	void AttachCamera();
};
