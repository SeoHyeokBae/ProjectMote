// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UIInfo.h"
#include "../../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "MonsterInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UMonsterInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UTextBlock*		mMonsterName;
	UProgressBar*	mMonsterHP;
	UProgressBar*	mDelayHP;
	UProgressBar*	mMonsterStagger;
	UProgressBar*	mDelayStagger;

	TObjectPtr<class AAIMonsterBase> mCurrentTarget;
	TObjectPtr<class AAIMonsterBase> mPrevTarget;

	float mCurDelayHP = 1.f;
	float mCurStaggerHP = 1.f;

	FTimerHandle TimerHandleHP;
	FTimerHandle TimerHandleStagger;

private:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& Geometry, float DeltaTime);

public:
	void SetMonsterName(const FString& Name);
	void SetMonsterHP(float Percent);
	void SetMonsterStagger(float Percent);
	void SetTarget(class AAIMonsterBase* NewMonster);
	void ClearTarget();
	void ClearHandle();
};
