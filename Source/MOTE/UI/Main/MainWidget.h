// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UIInfo.h"
#include "../../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct();
	
public:
	UPROPERTY()
	UImage* mSkillStoneImage;

	UPROPERTY()
	UImage* mSkillStoneBack;
	UPROPERTY()
	UImage* mSkillStoneFrame;
	UPROPERTY()
	UUserWidget* mPlayerHP;
	UPROPERTY()
	class UNiagaraSystemWidget* mNiagaraWidget;

	UPROPERTY()
	class UNiagaraSystemWidget* mNiagaraStoneLight;

	UFUNCTION(BlueprintCallable)
	void AppearWidget();
	UFUNCTION(BlueprintCallable)
	void DisappearWidget();
	UFUNCTION(BlueprintCallable)
	void ShowGuide();
	UFUNCTION(BlueprintCallable)
	void HideGuide();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeInGuide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOutGuide;
};
