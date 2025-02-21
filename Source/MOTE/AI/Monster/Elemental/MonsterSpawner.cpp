// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Elemental/MonsterSpawner.h"
#include "AI/Monster/Elemental/LightElemental.h"
#include "../../../Controller/MonsterController.h"
#include "NavigationSystem.h"

// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		ElementalSpawnAsset(TEXT("/Script/Engine.ParticleSystem'/Game/FX/InfinityBladeEffects/Effects/FX_Skill_RockBurst/P_RBurst_Fire_Charge_01.P_RBurst_Fire_Charge_01'"));

	if (ElementalSpawnAsset.Succeeded())
		ElementalSpawnEffect = ElementalSpawnAsset.Object;

	SetRootComponent(mRoot);
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, [this]()
		{
			GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, [this]()
				{
					SpawnRandomLocation();
					++mCount;
				}, mInterval, true);
		}, 5.f, false);


}

// Called every frame
void AMonsterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mCount > mMaxSpawn)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);	
		Destroy();
	}
}

void AMonsterSpawner::SpawnRandomLocation()
{
	// ������ ���� Ŭ����

	// ������ġ ������ ���� ����޽�
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSystem)
	{
		FVector BaseLocation = GetActorLocation(); // �Ǵ� �÷��̾� ��ġ, Ư�� ����Ʈ ��

		// ���� ��ġ�� ã�� ���� �ݰ�
		mSpawnRadius = 5000.0f;

		// �Լ��� ��ȯ�ϴ� ���� ��ġ��
		// FNavLocation.Location 
		FNavLocation RandomLocation;
		
		// �׺���̼� �����Ͱ� �ִ��� Ȯ���մϴ�.
		if (NavSystem->GetRandomReachablePointInRadius(BaseLocation, mSpawnRadius, RandomLocation))
		{
			// ���� �Ķ���� ����
			FActorSpawnParameters SpawnParams;


			//RandomLocation.Location += FVector((RandomLocation.Location - PlayerCharacter->GetPlanetGravityDir()).Normalize() * 5000.0f);

			// ���� ��ġ�� ��ø�Ǿ� �浹�� ���ϵ��� ��ġ�� �����Ͽ� ����
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// ���� ����
			AAICharacterBase* DefaultPawn = mSpawnClass->GetDefaultObject<AAICharacterBase>();

			if (IsValid(DefaultPawn))
			{
				// ��ġ�� ȸ�� �� ����
				float RandomYaw = 30.0 * FMath::FRandRange(0.0,11.0);
				FRotator Rotation = FRotator(0.0,RandomYaw,0.0);
				FTransform SpawnTransform(Rotation, RandomLocation);
				
				mSpawnActor = GetWorld()->SpawnActor<AAICharacterBase>(mSpawnClass,
					SpawnTransform, SpawnParams);

				AMonsterController* contrl = Cast<AMonsterController>(mSpawnActor->GetController());
				contrl->SetGenericTeamId(FGenericTeamId(10));

				// LightElemental �Ͻ� ��������Ʈ
				if (Cast<ALightElemental>(mSpawnActor))
				{
					SpawnTransform.SetLocation(RandomLocation.Location + FVector(0, 0, 150));
					SpawnTransform.SetScale3D(FVector(4, 4, 4));
					UGameplayStatics::SpawnEmitterAtLocation(mSpawnActor->GetWorld(), ElementalSpawnEffect, SpawnTransform);
				}
			}
		}
	}
}

