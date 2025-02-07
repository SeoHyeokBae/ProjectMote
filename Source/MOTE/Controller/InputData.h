// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "UObject/NoExportTypes.h"
#include "InputData.generated.h"

UCLASS()
class MOTE_API UDefaultInputData : public UObject
{
	GENERATED_BODY()
	
public:
	UDefaultInputData();

public:
	UInputMappingContext* mDefaultContext = nullptr;

public:
	UInputAction* mMove = nullptr;
	UInputAction* mRotation = nullptr;
	UInputAction* mAttack = nullptr;
	UInputAction* mJump = nullptr;
	UInputAction* mDash = nullptr;
	UInputAction* mFloat = nullptr;
	UInputAction* mAim = nullptr;
	UInputAction* mDetected = nullptr;

	UInputAction* mBossKill = nullptr;
	UInputAction* mGuide = nullptr;
	UInputAction* mInteraction = nullptr;
	UInputAction* mPause = nullptr;
	UInputAction* mRespawn = nullptr;
};
