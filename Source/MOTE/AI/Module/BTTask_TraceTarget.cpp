// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/BTTask_TraceTarget.h"
#include "../AICharacterBase.h"
#include "AIController.h"

UBTTask_TraceTarget::UBTTask_TraceTarget()
{
	NodeName = TEXT("TraceTarget");
	AcceptableRadius = 300.f;

	//bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_TraceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// Target을 얻어온다.
	AAICharacterBase* AICharacter = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();
	if (AICharacter)
	{
		AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
		if (Target)
		{
			AICharacter->SetAIType(EAIType::Trace);
		}
	}
	
	return Result;
}

EBTNodeResult::Type UBTTask_TraceTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
		AAICharacterBase* AIPawn = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();
		if (AIPawn)
		{
			AIPawn->SetAIType(EAIType::Idle);
		}
		return EBTNodeResult::Aborted;
}

void UBTTask_TraceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// AIPawn을 얻어온다.
	AAICharacterBase* AIPawn = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();

	if (!IsValid(AIPawn))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		AIPawn->SetAIType(EAIType::Idle);

		return;
	}

	// Target을 얻어온다.
	AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!IsValid(Target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		AIPawn->SetAIType(EAIType::Idle);
		return;
	}

	float Dist = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance"));
	if (Dist > 280.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		AIPawn->SetAIType(EAIType::Idle);
		return;
	}

	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTTask_TraceTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	AAICharacterBase* AIPawn = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();
	if (AIPawn)
	{
		AIPawn->SetAIType(EAIType::Idle);
	}


}
