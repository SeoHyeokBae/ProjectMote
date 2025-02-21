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

		// ���� ������
		mLockOnLaser = Golem->GetWorld()->SpawnActor<ALockOnLaser>(ALockOnLaser::StaticClass(), Golem->GetActorTransform(), SpawnParams);
		if (mLockOnLaser)
		{
			mLockOnLaser->SetOwner(Golem);
			mLockOnLaser->AttachToComponent(Golem->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "LazerSocket");

			AActor* Target = Cast<AActor>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
			if (Target) 
			{
				FVector PlayerLoc = Target->GetActorLocation() - Target->GetActorUpVector() * 20.f;
				FVector Start;
				FRotator Rot;
				Golem->GetMesh()->GetSocketWorldLocationAndRotation("LazerSocket", Start, Rot);

				//FRotator TargetRot = (PlayerLoc - Start).GetSafeNormal().Rotation();
				FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(Start, PlayerLoc);
				mLockOnLaser->SetActorRotation(TargetRot + FRotator(180, 0, 0));
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("Vector: %s"), *TargetRot.ToString()));
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("Vector: %s"), *Rot.ToString()));
				mPrevRot = TargetRot;
			}
		}
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_SkillLazer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	// AIPawn�� ���´�.
	AGolem* Golem = OwnerComp.GetAIOwner()->GetPawn<AGolem>();

	if (!IsValid(Golem))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

		Golem->SetGolemAnim(EGolemState::Idle);

		return;
	}

	// Target�� ���´�.
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

	// Ÿ�� ����
	if (mLockOnLaser )
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

		// �� ���� ����
		FRotator PrevRot = mPrevRot;
		//FRotator TargetRot = (PlayerLoc - Start).GetSafeNormal().Rotation();
		FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(Start, PlayerLoc);

		// ȸ�� �ӵ� ����
		//mRotSpeed < 35.f ? mRotSpeed += DeltaSeconds * 10.f : mRotSpeed = 20.f;
		//const FRotator InterpToRotator = FMath::RInterpTo(PrevRot, TargetRot, DeltaSeconds, mRotSpeed);

		// ȸ�� �ӵ� ����
		mRotAcc += DeltaSeconds / 2.5f;							// 2.5�� �ְ���
		float frequency = FMath::Clamp(mRotAcc + 1.5, 1.5, 2);	// 1.5PI ~ 2PI ���� 
		mRotSpeed = (FMath::Sin(frequency * PI) + 1) * 40.f;	// �ִ밪 40
		const FRotator InterpToRotator = FMath::RInterpTo(PrevRot, TargetRot, DeltaSeconds, mRotSpeed);

		
		// ���� ���а� �ƴ϶�� ��� ����
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

	//if (!Target && mTimeOver < mTimeAcc)
	//{
	//	if (mLockOnLaser)
	//		mLockOnLaser->Destroy();

	//	Golem->SetGolemAnim(EGolemState::Skill_Lazer);
	//}

	// ���ݳ������� �Ǵ�
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
	mRotSpeed = 0.f;
	mRotAcc = 0.f;
}
