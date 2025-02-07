// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "GameFramework/Actor.h"
#include "LockOnLaser.generated.h"

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

	float mLockOnTime		= 2.5f; // 0.5
	float mLockOnAcc		= 0.f;
	float mMissTargetTime	= 3.f; // 0.3
	float mMissTargetAcc	= 0.f;
	float mTotalAcc			= 0.f;

	float DefaultColorParam	= 10.f;

	bool mIsLockOn = false;
	bool mIsFinish = false;

public:
	bool IsLockOn() const { return mIsLockOn; }
	bool IsFinish() const { return mIsFinish; }

protected:
	FTimerHandle DelayTimerHandle;
	FTimerHandle FireTimerHandle;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	void UpdateLength();

};
