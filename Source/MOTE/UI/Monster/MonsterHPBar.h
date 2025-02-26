// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UIInfo.h"
#include "../../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHPBar.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UMonsterHPBar : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY()
	UProgressBar* mMonsterHP;
	UPROPERTY()
	UProgressBar* mDelayHP;

	UPROPERTY()
	class AAIMonsterBase* mOwner;

	FTimerHandle TimerHandleHP;

private:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime);

public:
	void SetMonsterHP(float Percent);
	void SetOwner(class AAIMonsterBase* Owner);
	void ClearHandle();

protected:
	void DelayReductionEffect();
};
