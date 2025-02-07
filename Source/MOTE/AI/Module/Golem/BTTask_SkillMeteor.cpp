// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/Golem/BTTask_SkillMeteor.h"
#include "AI/Monster/Golem/MeteorSpawnActor.h"
#include "AI/AICharacterBase.h"
#include "AIController.h"
#include "../../Monster/Golem/Golem.h"
#include "../../../Player/PlayerCharacter.h"

UBTTask_SkillMeteor::UBTTask_SkillMeteor()
{
	// 메테오 추락 시간동안 loop 애니메이션
	NodeName = TEXT("SkillMeteor");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_SkillMeteor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		Golem->SetGolemAnim(EGolemState::Skill_Meteor);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_SkillMeteor::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// AIPawn을 얻어온다.
	AGolem* Golem = OwnerComp.GetAIOwner()->GetPawn<AGolem>();
	if (!IsValid(Golem))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Golem->SetGolemAnim(EGolemState::Idle);
		return;
	}

	// Target을 얻어온다.
	AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	//if (!Target)
	//{
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	//	Golem->SetGolemAnim(EGolemState::Idle);

	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NoTarget"));
	//	return;
	//}

	bool IsStagger = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(TEXT("Stagger"));
	if (IsStagger)
	{
		OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		Golem->SetGolemAnim(EGolemState::Idle);
		return;
	}

	if (Golem->GetCanSpawnMeteor())
	{

		APlayerCharacter* Player = Cast<APlayerCharacter>(Target);
		if (Player)
		{
			Golem->SetSpawnMeteor();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// 위치와 회전 값 설정
			FRotator Rotation = FRotator::ZeroRotator;
			FVector TargetLocation = Player->GetActorLocation();
			if (Player->GetPlanetGravityDir() != FVector(0.f, 0.f, -1.0f))
			{
				TargetLocation += Player->GetActorUpVector() * 10000.0f;
			}
			else
			{
				TargetLocation += FVector(0, 0, 10000.0f);
			}
			// 액터 스폰
			AMeteorSpawnActor* SpawnedActor = GetWorld()->SpawnActor<AMeteorSpawnActor>(AMeteorSpawnActor::StaticClass(), TargetLocation, Rotation, SpawnParams);
			if (Golem->IsRage)
				SpawnedActor->RageMode();
		}
	}

	bool CanAttack = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(TEXT("CanAttack"));
	if (!CanAttack)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		Golem->SetGolemAnim(EGolemState::Idle);

		//OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);
		return;
	}

}

void UBTTask_SkillMeteor::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
