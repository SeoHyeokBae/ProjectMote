// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerPauseWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UPlayerPauseWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(VisibleAnywhere)
	class UButton* ResumeButton;

	UPROPERTY(VisibleAnywhere)
	UButton* OptionButton;

	UPROPERTY(VisibleAnywhere)
	UButton* QuitButton;

	UPROPERTY(VisibleAnywhere)
	class UImage* SelectResume;

	UPROPERTY(VisibleAnywhere)
	UImage* SelectOption;

	UPROPERTY(VisibleAnywhere)
	UImage* SelectQuit;

private:
	UFUNCTION()
	void ResumeButtonClick();

	UFUNCTION()
	void OptionButtonClick();

	UFUNCTION()
	void QuitButtonClick();

	UFUNCTION()
	void ResumeButtonHover();

	UFUNCTION()
	void OptionButtonHover();

	UFUNCTION()
	void QuitButtonHover();

	UFUNCTION()
	void ResumeButtonUnhover();

	UFUNCTION()
	void OptionButtonUnhover();

	UFUNCTION()
	void QuitButtonUnhover();

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};
