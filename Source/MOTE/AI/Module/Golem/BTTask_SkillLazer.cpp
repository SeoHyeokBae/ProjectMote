// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Module/Golem/BTTask_SkillLazer.h"
#include "../../AICharacterBase.h"
#include "../../Monster/Golem/Golem.h"
#include "../../Monster/Golem/LockOnLaser.h"
#include "../../../Player/PlayerCharacter.h"
#include "AIController.h"

UBTTask_SkillLazer::UBTTask_SkillLazer()
{
	NodeName = TEXT("SkillLazer");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_SkillLazer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AGolem* Golem = OwnerComp.GetAIOwner()->GetPawn<AGolem>();

	if (IsValid(Golem))
	{
		AAIController* Control = Golem->GetController<AAIController>();
		if (IsValid(Control))
		{
			//Control->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);

			bool IsStagger = Control->GetBlackboardComponent()->GetValueAsBool(TEXT("Stagger"));
			if (IsStagger)
			{
				return EBTNodeResult::Failed;
			}
		}

		Golem->SetGolemAnim(EGolemState::Idle);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 조준 레이저
		mLockOnLaser = Golem->GetWorld()->SpawnActor<ALockOnLaser>(ALockOnLaser::StaticClass(), Golem->GetActorTransform(), SpawnParams);
		if (mLockOnLaser)
		{
			mLockOnLaser->SetOwner(Golem);
			mLockOnLaser->AttachToComponent(Golem->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "LazerSocket");
			mLockOnLaser->SetActorRelativeRotation(FRotator(0, 180, 0));
			mPrevRot = mLockOnLaser->GetActorRotation();
		}
	}



	return EBTNodeResult::InProgress;
}

void UBTTask_SkillLazer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	if (!Target && mPrevLoc == FVector::ZeroVector)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		Golem->SetGolemAnim(EGolemState::Idle);

		return;
	}

	AAIController* Control = Golem->GetController<AAIController>();
	if (IsValid(Control))
	{
		bool IsStagger = Control->GetBlackboardComponent()->GetValueAsBool(TEXT("Stagger"));
		if (IsStagger)
		{
			if (mLockOnLaser)
				mLockOnLaser->Destroy();

			Control->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			Golem->SetGolemAnim(EGolemState::Idle);

			return;
		}
	}

	// 타켓 조준
	if (mLockOnLaser)
	{
		FVector PlayerLoc;
		APlayerCharacter* Player = Cast<APlayerCharacter>(Target);
		if (Player)
		{
			mTimeAcc = 0.f;
			PlayerLoc = Player->GetActorLocation() - Player->GetActorUpVector() * 20.f;
			mPrevLoc = PlayerLoc;
		}
		else
		{
			mTimeAcc += DeltaSeconds;
			PlayerLoc = mPrevLoc;
		}

		FVector Loc;
		FRotator Rot;
		Golem->GetMesh()->GetSocketWorldLocationAndRotation("LazerSocket", Loc, Rot);
		FVector Start = Loc;

		// 빔 방향 변경
		FRotator PrevRot = mPrevRot;
		FRotator TargetRot = (PlayerLoc - Start).GetSafeNormal().Rotation();

		// 회전 속도 증가
		mRotSpeed < 20.f ? mRotSpeed += DeltaSeconds * 10.f : mRotSpeed = 20.f;

		const FRotator InterpToRotator = FMath::RInterpTo(PrevRot, TargetRot, DeltaSeconds, mRotSpeed);
		// 락온 실패가 아니라면 계속 추적
		if (!(mLockOnLaser->IsFinish() && !mLockOnLaser->IsLockOn()))
		{
			if (mLockOnLaser->IsLockOn())
			{
				Golem->SetLaserAimRot(TargetRot);
			}
			else
			{
				Golem->SetLaserAimRot(InterpToRotator);
				mLockOnLaser->SetActorRotation(InterpToRotator + FRotator(180, 0, 0)); // Offset 
			}
		}
		mPrevRot = InterpToRotator;
	}

	if (!Target && mTimeOver < mTimeAcc)
	{
		if (mLockOnLaser)
			mLockOnLaser->Destroy();

		Golem->SetGolemAnim(EGolemState::Skill_Lazer);
	}

	// 공격끝났는지 판단
	bool CanAttack = OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsBool(TEXT("CanAttack"));
	if (!CanAttack)
	{
		if (mLockOnLaser)
			mLockOnLaser->Destroy();

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		Golem->SetGolemAnim(EGolemState::Idle);

		//OwnerComp.GetAIOwner()->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);

		return;
	}


}

void UBTTask_SkillLazer::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

}
