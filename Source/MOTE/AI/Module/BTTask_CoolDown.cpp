// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/BTTask_CoolDown.h"
#include "AI/AIInfo.h"
#include "AIController.h"

UBTTask_CoolDown::UBTTask_CoolDown()
{
	NodeName = TEXT("Attack Cool Down");
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_CoolDown::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory); 

	return EBTNodeResult::InProgress; 
}

void UBTTask_CoolDown::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);

}
