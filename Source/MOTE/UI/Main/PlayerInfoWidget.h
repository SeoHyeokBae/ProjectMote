// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UIInfo.h"
#include "../../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UPlayerInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UProgressBar* mPlayerHP;
	UProgressBar* mDelayHP;

	FTimerHandle TimerHandleHP;

private:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime);

public:
	void SetHPPercent(float Percent);
	void ClearHandle();
};
