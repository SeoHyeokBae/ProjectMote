// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	class UButton* AcceptButton;

	UPROPERTY(VisibleAnywhere)
	class UButton* MasterMuteBotton;

	UPROPERTY(VisibleAnywhere)
	class UButton* BGMMuteBotton;

	UPROPERTY(VisibleAnywhere)
	class UButton* VFXMuteBotton;

	UPROPERTY(VisibleAnywhere)
	class USlider* MasterSlider;

	UPROPERTY(VisibleAnywhere)
	class USlider* BGMSlider;

	UPROPERTY(VisibleAnywhere)
	class USlider* VFXSlider;

	bool IsMasterMute = false;
	bool IsBGMMute = false;
	bool IsVFXMute = false;

	float PrevMasterVolume;
	float PrevBGMVolume;
	float PrevVFXVolume;

	UUserWidget* prevWidget;
	FLinearColor TempColor = FLinearColor(1.f,1.f,1.f);

	class USoundSubsystem* SoundSystem;
private:
	UFUNCTION()
	void AcceptButtonClick();
	UFUNCTION()
	void AcceptButtonHover();
	UFUNCTION()
	void AcceptButtonUnhover();

	UFUNCTION()
	void MasterMuteClick();
	UFUNCTION()
	void MasterMuteHover();
	UFUNCTION()
	void MasterMuteUnhover();

	UFUNCTION()
	void BGMMuteClick();
	UFUNCTION()
	void BGMMuteHover();
	UFUNCTION()
	void BGMMuteUnhover();

	UFUNCTION()
	void VFXMuteClick();
	UFUNCTION()
	void VFXMuteHover();
	UFUNCTION()
	void VFXMuteUnhover();

	UFUNCTION()
	void MasterValueChange(float InValue);
	UFUNCTION()
	void MasterSlideCaputeBegin();
	UFUNCTION()
	void MasterSlideCaputeEnd();

	UFUNCTION()
	void BGMValueChange(float InValue);
	UFUNCTION()
	void BGMSlideCaputeBegin();
	UFUNCTION()
	void BGMSlideCaputeEnd();

	UFUNCTION()
	void VFXValueChange(float InValue);
	UFUNCTION()
	void VFXSlideCaputeBegin();
	UFUNCTION()
	void VFXSlideCaputeEnd();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* AcceptHoverAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* MasterHoverAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* BGMHoverAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* VFXHoverAnim;

public:
	FORCEINLINE void SetPrevWidget(UUserWidget* Widget) { prevWidget = Widget; }
	void UpdateSlide(float MasterVolume, float BGMVolume, float VFXVolume);
};
