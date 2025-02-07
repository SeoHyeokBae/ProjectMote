// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/BTTask_Patrol.h"
#include "../AICharacterBase.h"
#include "AIController.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");

	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAICharacterBase* AIPawn = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();

	if (IsValid(AIPawn))
		AIPawn->SetAIType(EAIType::Patrol);

	return Result;
}

EBTNodeResult::Type UBTTask_Patrol::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::AbortTask(OwnerComp, NodeMemory);

	return Result;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Target을 얻어온다.
	AAICharacterBase* AICharacter = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();
	if (AICharacter)
	{
		AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
		if (Target)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

			AICharacter->SetAIType(EAIType::Idle);

			return;
		}
	}

	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTTask_Patrol::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	// 패트롤 포인트를 다음으로 갱신한다.
	AAICharacterBase* AICharacter = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();
	if (IsValid(AICharacter))
	{
		AICharacter->NextPatrolVectorPoint();
		AICharacter->RegisterPatrolVectorPoint();

		AICharacter->SetAIType(EAIType::Idle);
	}
}
