// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../AIInfo.h"
#include "../../GameInfo.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UBTTask_Patrol : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_Patrol();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
};
