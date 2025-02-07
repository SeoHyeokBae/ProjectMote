// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/BTTask_Attack.h"
#include "../AICharacterBase.h"
#include "AIController.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AAICharacterBase* AIPawn = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();

	if (IsValid(AIPawn))
		AIPawn->SetAIType(EAIType::Attack);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_Attack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::AbortTask(OwnerComp, NodeMemory);

	return Result;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// AIPawn�� ���´�.
	AAICharacterBase* AIPawn = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();

	if (!IsValid(AIPawn))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		AIPawn->SetAIType(EAIType::Idle);

		return;
	}


	// Target�� ���´�.
	AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));

	if (!IsValid(Target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		AIPawn->SetAIType(EAIType::Idle);

		return;
	}

	bool Hit = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(TEXT("TakeDamage"));
	if (Hit)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		AIPawn->SetAIType(EAIType::Idle);

		return;
	}

	// ���ݳ������� �Ǵ�
	bool AttackEnd = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(TEXT("AttackEnd"));
	if (AttackEnd)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		AIPawn->SetAIType(EAIType::Idle);

		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool(TEXT("AttackEnd"), false);

		// ���ݰŸ��� ������������ �Ǵ��Ѵ�
		FVector AILocation = AIPawn->GetActorLocation();
		FVector TargetLocation = Target->GetActorLocation();

		AILocation.Z -= AIPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		float Radius = AIPawn->GetCapsuleComponent()->GetScaledCapsuleRadius();

		UCapsuleComponent* TargetCapsule = Cast<UCapsuleComponent>(Target->GetRootComponent());
		if (IsValid(TargetCapsule))
		{
			TargetLocation.Z -= TargetCapsule->GetScaledCapsuleHalfHeight();
			Radius += TargetCapsule->GetScaledCapsuleRadius();
		}

		// �� �� ������ �Ÿ��� ���Ѵ�
		float Distance = FVector::Distance(AILocation, TargetLocation);
		if (Distance > 150.f + Radius)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

void UBTTask_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

}
