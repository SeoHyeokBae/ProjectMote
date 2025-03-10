// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../GameInfo.h"
#include "GameFramework/Actor.h"
#include "DmgTextActor.generated.h"

#define FREQUENCY 1.5f // ��
#define AMPLITUDE 90   // ����

UCLASS()
class MOTE_API ADmgTextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADmgTextActor();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* mRoot;

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* mWidgetComponent;

	UPROPERTY(VisibleAnywhere)
	class UDmageAmountWidget* mWidget;

	// ����
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UTimelineComponent>	mTimelineComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCurveFloat>			mTimeCurve;

	UPROPERTY(VisibleAnywhere)
	FVector StartLocation;

	UPROPERTY(VisibleAnywhere)
	FVector EndLocation;

	UPROPERTY(VisibleAnywhere)
	FVector IncreaseEndLocation;

	bool IsIncrease = false;
	float TimeAcc = 0.f;

public:
	float mDamageAmount;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void InputDamage(float Damage, bool IsHeadShot = false);
	void ChangeColor(FLinearColor NewColor);

	void SetIsIncrease() { IsIncrease = true; }

protected:
	UFUNCTION()
	void TimeLineUpdate(float Alpha);
	UFUNCTION()
	void TimeLineFinish();
};
