// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/Golem/BTTask_SkillCrash.h"
#include "../../AICharacterBase.h"
#include "../../Monster/Golem/Golem.h"
#include "AIController.h"

UBTTask_SkillCrash::UBTTask_SkillCrash()
{
	NodeName = TEXT("SkillCrash");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_SkillCrash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AGolem* Golem = OwnerComp.GetAIOwner()->GetPawn<AGolem>();

	if (IsValid(Golem))
	{
		AAIController* Control = Golem->GetController<AAIController>();
		if (IsValid(Control))
		{
			bool IsStagger = Control->GetBlackboardComponent()->GetValueAsBool(TEXT("Stagger"));
			if (IsStagger)
			{
				return EBTNodeResult::Failed;
			}
		}

		Golem->SetGolemAnim(EGolemState::Skill_Crash);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_SkillCrash::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);


	AGolem* Golem = OwnerComp.GetAIOwner()->GetPawn<AGolem>();

	if (!IsValid(Golem))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Golem->SetGolemAnim(EGolemState::Idle);

		return;
	}

	AAIController* Control = Golem->GetController<AAIController>();
	if (IsValid(Control))
	{
		bool IsStagger = Control->GetBlackboardComponent()->GetValueAsBool(TEXT("Stagger"));
		if (IsStagger)
		{
			Control->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			Golem->SetGolemAnim(EGolemState::Idle);

			return;
		}
	}

	bool CanAttack = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(TEXT("CanAttack"));
	if (!CanAttack)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		Golem->SetGolemAnim(EGolemState::Idle);

		return;
	}


}

void UBTTask_SkillCrash::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

}
