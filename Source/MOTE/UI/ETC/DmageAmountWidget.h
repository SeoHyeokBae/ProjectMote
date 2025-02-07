// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DmageAmountWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UDmageAmountWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DmgAmt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Fade;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeHeadShotText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DmgText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HeadShotText;

protected:
	virtual void NativeConstruct() override;

public:
	// Text에 바인딩 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	FText GetDynamicText();

	void FadeText();
	void FadeHeadShot();
	void ChangeColor(FLinearColor NewColor);
	void ChangeScale(float DmgAmount);
};
