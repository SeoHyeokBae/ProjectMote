// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/BTTask_TurnToTarget.h"
#include "AIController.h"
#include "AI/AICharacterBase.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI
	AAICharacterBase* AICharacter = OwnerComp.GetAIOwner()->GetPawn<AAICharacterBase>();
	if (!AICharacter)
	{
		return EBTNodeResult::Failed;
	}
	// 플레이어(Target)
	AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}
	// 회전
	FVector LockVector = Target->GetActorLocation() - AICharacter->GetActorLocation();
	LockVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LockVector).Rotator();
	
	if (AICharacter->GetAIType() == EAIType::Boss && !OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(TEXT("Stagger")))
	{
		AICharacter->SetActorRotation(FMath::RInterpTo(AICharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 1.25f));
	}
	else
	{
		AICharacter->SetActorRotation(FMath::RInterpTo(AICharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 3.0f));
	}
	
	//return EBTNodeResult::Succeeded;
	return Result;
}
