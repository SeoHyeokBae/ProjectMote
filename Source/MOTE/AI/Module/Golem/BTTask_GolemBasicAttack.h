// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "../../AIInfo.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GolemBasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UBTTask_GolemBasicAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_GolemBasicAttack();
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
};
