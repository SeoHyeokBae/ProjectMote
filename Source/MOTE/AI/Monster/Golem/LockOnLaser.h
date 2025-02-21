// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "GameFramework/Actor.h"
#include "LockOnLaser.generated.h"

#define CylinerSize 200.f

UCLASS()
class MOTE_API ALockOnLaser : public AActor
{
	GENERATED_BODY()
	
public:	
	ALockOnLaser();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent*		mRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent*	mMesh;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent*		mArrow;


	UPROPERTY(VisibleAnywhere)
	UMaterialInstance* mMtrlInst;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* mMtrlInstDynamic;

	float mLockOnTime		= 1.5f; // 0.5
	float mLockOnAcc		= 0.f;
	float mMissTargetTime	= 3.f; // 0.3
	float mTotalAcc			= 0.f;
	float mMissTargetAcc = 0.f;
	float DefaultColorParam	= 10.f;

	float mStartTime = 0.f;

	bool mIsLockOn = false;
	bool mIsFinish = false;

public:
	bool IsLockOn() const { return mIsLockOn; }
	bool IsFinish() const { return mIsFinish; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float Deltatime) override;

protected:
	void UpdateLength(float Deltatime);

};
