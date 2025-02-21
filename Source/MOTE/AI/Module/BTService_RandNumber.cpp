// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/BTService_RandNumber.h"
#include "AIController.h"

UBTService_RandNumber::UBTService_RandNumber()
{
	NodeName = TEXT("CreateRandomNumber");
	Interval = 1.0f;

	bNotifyBecomeRelevant = true;
}

void UBTService_RandNumber::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	int Num = 0;
	Num = FMath::RandRange(1, 100);

	if (Num <= 10)			// 10%
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(CMonsterDefaultKey::mAttackNumber, 1);
	}
	else if (Num <= 20)		// 10%
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(CMonsterDefaultKey::mAttackNumber, 2);
	}
	else if (Num <= 45)		// 25%
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(CMonsterDefaultKey::mAttackNumber, 3);
	}
	else if (Num <= 70)		// 25%
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(CMonsterDefaultKey::mAttackNumber, 4);
	}
	else if (Num <= 85)		// 15%
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(CMonsterDefaultKey::mAttackNumber, 5);
	}
	else					// 15%
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(CMonsterDefaultKey::mAttackNumber, 6);
	}
}
