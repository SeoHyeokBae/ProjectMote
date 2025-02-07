// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UIInfo.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDeathWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UPlayerDeathWidget : public UUserWidget
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
	class UButton* ReStartButton;

	UPROPERTY(VisibleAnywhere)
	class UButton* ExitButton;

	UPROPERTY(VisibleAnywhere)
	UImage* SelectReStart;

	UPROPERTY(VisibleAnywhere)
	UImage* SelectExit;

	UPROPERTY(VisibleAnywhere)
	UImage* BackGround;

	UPROPERTY(VisibleAnywhere)
	UBackgroundBlur* Blur;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;

private:
	UFUNCTION()
	void ReStartButtonClick();

	UFUNCTION()
	void ExitButtonClick();

	UFUNCTION()
	void ReStartButtonHover();

	UFUNCTION()
	void ExitButtonHover();

	UFUNCTION()
	void ReStartButtonUnhover();

	UFUNCTION()
	void ExitButtonUnhover();

public:
	void AppearWidget();
};
