// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/GolemController.h"
#include "../AI/Monster/Golem/Golem.h"

AGolemController::AGolemController()
{
	mAIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	// AIController�� PerceptionComponent�� ����� �� �ְ� �Ǿ� �ִ�
	SetPerceptionComponent(*mAIPerception);

	// �þ߸� ������ش�
	mSightConfing = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// backoffset
	mSightConfing->SetMaxAge(5.f);
	mSightConfing->SightRadius = 50000.f;
	mSightConfing->LoseSightRadius = 100000.f;
	mSightConfing->PeripheralVisionAngleDegrees = 360.f;
	mSightConfing->AutoSuccessRangeFromLastSeenLocation = -1.f;
	mSightConfing->DetectionByAffiliation.bDetectEnemies = true;
	mSightConfing->DetectionByAffiliation.bDetectNeutrals = false;
	mSightConfing->DetectionByAffiliation.bDetectFriendlies = false;

	// �迭�� �߰��Ѵ�, Ư�������� �ҽ��� ���
	mAIPerception->ConfigureSense(*mSightConfing);

	// ��ǥ�������
	mAIPerception->SetDominantSense(mSightConfing->GetSenseImplementation());

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		AITreeAsset(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/Monster/BT_Monster_Golem.BT_Monster_Golem'"));

	if (AITreeAsset.Succeeded())
		mAITree = AITreeAsset.Object;

	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		BlackboardAsset(TEXT("/Script/AIModule.BlackboardData'/Game/AI/Monster/BB_Monster_Golem.BB_Monster_Golem'"));
	
	if (BlackboardAsset.Succeeded())
		mBlackBoard = BlackboardAsset.Object;
}

FGenericTeamId AGolemController::GetGenericTeamId() const
{
	return FGenericTeamId(mTeamID);
}

ETeamAttitude::Type AGolemController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);

	if (!OtherTeamAgent)
		return ETeamAttitude::Neutral;

	else if (OtherTeamAgent->GetGenericTeamId() == 255)
		return ETeamAttitude::Neutral;

	else if (GetGenericTeamId() == OtherTeamAgent->GetGenericTeamId())
		return ETeamAttitude::Friendly;

	return ETeamAttitude::Hostile;
}

void AGolemController::BeginPlay()
{
	Super::BeginPlay();

	mAIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AGolemController::GolemOnTargetDetect);
	mAIPerception->OnTargetPerceptionForgotten.AddDynamic(this, &AGolemController::GolemOnTargetForget);
}

void AGolemController::OnPossess(APawn* InPawn)
{
	if (IsValid(mAITree) && IsValid(mBlackBoard))
	{
		UBlackboardComponent* BlackboardRef = nullptr;
		if (UseBlackboard(mBlackBoard, BlackboardRef))
		{
			// BehaviorTree�� ���۽�Ų��
			RunBehaviorTree(mAITree);
		}
	}

	Super::OnPossess(InPawn);
}

void AGolemController::OnUnPossess()
{
	Super::OnUnPossess();

}

void AGolemController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGolemController::GolemOnTargetDetect(AActor* Target, FAIStimulus Stimulus)
{// ���ڷ� ������ Ÿ���� ���Ͱ� �ƴ� ��Ʈ�ѷ��̴�

	// ������ �ƴٸ�
	if (Stimulus.WasSuccessfullySensed())
	{
		AController* Controller = Cast<AController>(Target);

		if (IsValid(Controller))
		{
			Target = Controller->GetPawn<AActor>();

		}

		// Ÿ�� �ߺ��̿��� �ѹ��� ���� Ÿ���� ��������� ������
		// ���δٸ��� ���Ӱ� Ÿ������
		if (Target != Blackboard->GetValueAsObject(CMonsterDefaultKey::mTarget))
		{
			Blackboard->SetValueAsObject(CMonsterDefaultKey::mTarget, Target);
		}

	}
	else
	{
		Blackboard->SetValueAsObject(CMonsterDefaultKey::mTarget, nullptr);
	}
}

void AGolemController::GolemOnTargetForget(AActor* Target)
{
}
