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
	// 스폰할 액터 클래스

	// 랜덤위치 스폰을 위한 내비메쉬
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSystem)
	{
		FVector BaseLocation = GetActorLocation(); // 또는 플레이어 위치, 특정 포인트 등

		// 랜덤 위치를 찾기 위한 반경
		mSpawnRadius = 5000.0f;

		// 함수가 반환하는 랜덤 위치값
		// FNavLocation.Location 
		FNavLocation RandomLocation;
		
		// 네비게이션 데이터가 있는지 확인합니다.
		if (NavSystem->GetRandomReachablePointInRadius(BaseLocation, mSpawnRadius, RandomLocation))
		{
			// 스폰 파라미터 설정
			FActorSpawnParameters SpawnParams;


			//RandomLocation.Location += FVector((RandomLocation.Location - PlayerCharacter->GetPlanetGravityDir()).Normalize() * 5000.0f);

			// 스폰 위치가 중첩되어 충돌을 피하도록 위치를 조정하여 스폰
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// 액터 스폰
			AAICharacterBase* DefaultPawn = mSpawnClass->GetDefaultObject<AAICharacterBase>();

			if (IsValid(DefaultPawn))
			{
				// 위치와 회전 값 설정
				float RandomYaw = 30.0 * FMath::FRandRange(0.0,11.0);
				FRotator Rotation = FRotator(0.0,RandomYaw,0.0);
				FTransform SpawnTransform(Rotation, RandomLocation);
				
				mSpawnActor = GetWorld()->SpawnActor<AAICharacterBase>(mSpawnClass,
					SpawnTransform, SpawnParams);

				AMonsterController* contrl = Cast<AMonsterController>(mSpawnActor->GetController());
				contrl->SetGenericTeamId(FGenericTeamId(10));

				// LightElemental 일시 스폰이펙트
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

