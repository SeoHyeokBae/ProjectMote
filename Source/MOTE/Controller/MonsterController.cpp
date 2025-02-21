// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MonsterController.h"
#include "../Player/PlayerCharacter.h"
#include "Navigation/PathFollowingComponent.h"

AMonsterController::AMonsterController()
{
	SetGenericTeamId(FGenericTeamId(10));

	mAIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	// AIController�� PerceptionComponent�� ����� �� �ְ� �Ǿ� �ִ�
	SetPerceptionComponent(*mAIPerception);

	// �þ߸� ������ش�
	mSightConfing = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	// backoffset
	mSightConfing->SetMaxAge(3.f);
	mSightConfing->SightRadius = 1500.f;
	mSightConfing->LoseSightRadius = 1800.f;
	mSightConfing->PeripheralVisionAngleDegrees = 160.f;
	mSightConfing->DetectionByAffiliation.bDetectEnemies = true;
	mSightConfing->DetectionByAffiliation.bDetectNeutrals = false;
	mSightConfing->DetectionByAffiliation.bDetectFriendlies = false;
	
	// �迭�� �߰��Ѵ�, Ư�������� �ҽ��� ���
	mAIPerception->ConfigureSense(*mSightConfing);
	mDamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	mDamageConfig->SetMaxAge(3.0f);
	mAIPerception->ConfigureSense(*mDamageConfig);

	// ��ǥ�������
	mAIPerception->SetDominantSense(mSightConfing->GetSenseImplementation());
	
	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		AITreeAsset(TEXT("/Script/AIModule.BehaviorTree'/Game/AI/Monster/BT_MonsterDefault.BT_MonsterDefault'"));

	if (AITreeAsset.Succeeded())
		mAITree = AITreeAsset.Object;

	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		BlackboardAsset(TEXT("/Script/AIModule.BlackboardData'/Game/AI/Monster/BB_MonsterDefault.BB_MonsterDefault'"));

	if (BlackboardAsset.Succeeded())
		mBlackBoard = BlackboardAsset.Object;

}

FGenericTeamId AMonsterController::GetGenericTeamId() const
{
	return FGenericTeamId(mTeamID);
}

ETeamAttitude::Type AMonsterController::GetTeamAttitudeTowards(const AActor& Other) const
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

void AMonsterController::BeginPlay()
{
	Super::BeginPlay();

	mAIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterController::OnTargetDetect);
	mAIPerception->OnTargetPerceptionForgotten.AddDynamic(this, &AMonsterController::OnTargetForget);
}

void AMonsterController::OnPossess(APawn* InPawn)
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

void AMonsterController::OnUnPossess()
{
	Super::OnUnPossess();

}

void AMonsterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMonsterController::OnTargetDetect(AActor* Target, FAIStimulus Stimulus)
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
		if (Target != Blackboard->GetValueAsObject(TEXT("Target")))
		{
			Blackboard->SetValueAsObject(TEXT("Target"), Target);
			SetGenericTeamId(FGenericTeamId(10));

		}

	}
	else
	{
		AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("Target")));
		// MaxAge ���� Target�� ����������
		// Ÿ���� �Ҿ MaxAge���� BlackBoard�� ����
		if (CurrentTarget != Target)
		{
			Blackboard->SetValueAsObject(TEXT("Target"), nullptr);
			SetGenericTeamId(FGenericTeamId(10));
		}
	}
}

void AMonsterController::OnTargetForget(AActor* Target)
{
	/*
	DefaultEngine.ini
	[/Script/AIModule.AISystem]
	bForgetStaleActors=True
	*/

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("Target")));
	if (CurrentTarget == Target)
	{
		Blackboard->SetValueAsObject(TEXT("Target"), nullptr);
	}
}
