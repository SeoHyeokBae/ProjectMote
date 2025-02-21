// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SkillLazer.generated.h"

#define MAXT

UCLASS()
class MOTE_API UBTTask_SkillLazer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SkillLazer();

protected:
	UPROPERTY()
	class ALockOnLaser* mLockOnLaser;

	FVector mPrevLoc = FVector::ZeroVector;
	FRotator mPrevRot;
	float mRotSpeed = 0.0f;
	float mFireAcc = 0.f;
	float mFireTime = 1.5f;
	float mTimeOver = 3.f;
	float mTimeAcc = 0.f;
	float mRotAcc = 0.f;

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);


protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
	
};
