// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacterBase.h"
#include "AIController.h"
#include "NavigationSystem.h"

// Sets default values
AAICharacterBase::AAICharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = true;

	//GetMesh()->bReceivesDecals = false;
}


void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAICharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (mDeathDelegate.IsBound())
		mDeathDelegate.Broadcast(); 
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAICharacterBase::SetAIType(EAIType Type)
{
	mAIType = Type;
}

FGenericTeamId AAICharacterBase::GetGenericTeamId() const
{
	return FGenericTeamId(mTeamID);
}


void AAICharacterBase::AddPatrolStart(FVector Point)
{
	mPatrolPoints.Add(Point);
	RegisterPatrolVectorPoint();
}

void AAICharacterBase::RegisterPatrolVectorPoint()
{
	AAIController* Control = GetController<AAIController>();

	if (IsValid(Control))
	{
		bool	PatrolEnable = false;

		if (mPatrolPoints.Num() > 1)
		{
			PatrolEnable = true;
			Control->GetBlackboardComponent()->SetValueAsVector(
				CMonsterDefaultKey::mPatrolPoint, mPatrolPoints[mPatrolIndex]);
		}

		Control->GetBlackboardComponent()->SetValueAsBool(
			CMonsterDefaultKey::mPatrolEnable, PatrolEnable);
	}
}

void AAICharacterBase::NextPatrolVectorPoint()
{
	mPatrolIndex = (mPatrolIndex + 1) % mPatrolPoints.Num();
	
	if (0 == mPatrolIndex)
		RandomPatrolPoint();
}

void AAICharacterBase::RandomPatrolPoint()
{	
	// 랜덤위치 스폰을 위한 내비메쉬
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSystem)
	{
		FVector BaseLocation = GetActorLocation();

		// 랜덤 위치를 찾기 위한 반경
		float Radius = 2000.0f;

		// 기존 랜덤 포인트가 있으면 변경
		if (mPatrolPoints.Num() > 4)
		{
			for (int i = 1; i < 4; ++i)
			{
				// 함수가 반환하는 랜덤 위치값
				// FNavLocation.Location 
				FNavLocation RandomLocation;

				// 네비게이션 데이터가 있는지 확인
				if (NavSystem->GetRandomReachablePointInRadius(BaseLocation, Radius, RandomLocation))
				{
					mPatrolPoints[i] = RandomLocation.Location;
				}
			}
		}
		else
		{
			for (int i = 0; i < 3; ++i)
			{
				// 함수가 반환하는 랜덤 위치값
				FNavLocation RandomLocation;

				// 네비게이션 데이터가 있는지 확인
				if (NavSystem->GetRandomReachablePointInRadius(BaseLocation, Radius, RandomLocation))
				{
					mPatrolPoints.Add(RandomLocation.Location);
				}
			}
		}
	}
}

void AAICharacterBase::RegisterPatrolPoint()
{
	AAIController* Control = GetController<AAIController>();

	if (IsValid(Control))
	{
		bool	PatrolEnable = false;

		if (mPatrolArray.Num() > 1)
		{
			PatrolEnable = true;
			Control->GetBlackboardComponent()->SetValueAsObject(
				CMonsterDefaultKey::mPatrolPoint, mPatrolArray[mPatrolIndex]);
		}

		Control->GetBlackboardComponent()->SetValueAsBool(
			CMonsterDefaultKey::mPatrolEnable, PatrolEnable);
	}
}

void AAICharacterBase::NextPatrolPoint()
{
	mPatrolIndex = (mPatrolIndex + 1) % mPatrolArray.Num();
}

void AAICharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AAIController* Control = Cast<AAIController>(NewController);

	if (IsValid(Control))
	{
		bool	PatrolEnable = false;

		if (mPatrolArray.Num() > 1)
		{
			PatrolEnable = true;

			Control->GetBlackboardComponent()->SetValueAsObject(
				CMonsterDefaultKey::mPatrolPoint, mPatrolArray[mPatrolIndex]);
		}

		Control->GetBlackboardComponent()->SetValueAsBool(
			CMonsterDefaultKey::mPatrolEnable, PatrolEnable);
	}
}

float AAICharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void AAICharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAICharacterBase::DeathEnd()
{
	// ToDo
}

void AAICharacterBase::Create()
{
}

void AAICharacterBase::Load()
{
}

void AAICharacterBase::Init()
{
}
