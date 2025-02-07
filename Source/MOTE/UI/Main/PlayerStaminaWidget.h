// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UIInfo.h"
#include "../../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "PlayerStaminaWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UPlayerStaminaWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	UImage* UI_Stamina;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* mStaminaMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* mStaminaMaterialDynamic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeIn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* FadeOut;

protected:
	UPROPERTY(VisibleAnywhere)
	class APlayerCharacter* mPlayer;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float mMaxStamina = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float mCurrentStamina;

	float mDepleteStamina = 5.f;
	float mRefillStamina = 10.f;
	
	bool mIsFadeOut = false;

public:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime);

public:
	void SetStaminaPercent(float Percent);
};
