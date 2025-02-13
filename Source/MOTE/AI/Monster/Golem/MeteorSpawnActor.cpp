// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/MeteorSpawnActor.h"
#include "AI/Monster/Golem/GolemMeteor.h"
#include "NavigationSystem.h"
#include "../../../Controller/MotePlayerController.h"
#include "../../../Player/PlayerCharacter.h"

AMeteorSpawnActor::AMeteorSpawnActor()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SetRootComponent(mRoot);

}

void AMeteorSpawnActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMeteorSpawnActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	mTimeAcc += DeltaTime;
	if (mTimeAcc >= mInterval)
	{
		mTimeAcc = 0.f;
		SpawnRandomLocationNoNav();
		++mCount;
	}

	if (mCount > mMax)
		Destroy();
}

void AMeteorSpawnActor::RageMode()
{
	mMax = 8.f;
	CustomTimeDilation = 2.f;
}

void AMeteorSpawnActor::SpawnRandomLocation()
{	
	// 스폰할 액터 클래스
	TSubclassOf<AActor> ActorToSpawn = AGolemMeteor::StaticClass();

	// 랜덤위치 스폰을 위한 내비메쉬
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSystem)
	{
		FVector BaseLocation = GetActorLocation(); // 또는 플레이어 위치, 특정 포인트 등

		// 랜덤 위치를 찾기 위한 반경
		float Radius = 6000.0f;

		// 함수가 반환하는 랜덤 위치값
		// FNavLocation.Location 
		FNavLocation RandomLocation;

		// 네비게이션 데이터가 있는지 확인합니다.
		if (NavSystem->GetRandomReachablePointInRadius(BaseLocation, Radius, RandomLocation))
		{
			// 스폰 파라미터 설정
			FActorSpawnParameters SpawnParams;

			// 중력 방향 체크
			AMotePlayerController* MoteController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			
			if (MoteController)
			{
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MoteController->GetCharacter());
				if (PlayerCharacter->GetGravityDirection() != FVector(0.f,0.f,-1.0f) )
				{
					RandomLocation.Location += FVector((RandomLocation.Location - PlayerCharacter->GetGravityDirection()).Normalize() * 5000.0f);
				}
				else
				{
					RandomLocation.Location.Z += 3000.0f;
				}
			}
			// 스폰 위치가 중첩되어 충돌을 피하도록 위치를 조정하여 스폰
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// 위치와 회전 값 설정
			FRotator Rotation = FRotator::ZeroRotator;
			FTransform SpawnTransform(Rotation, RandomLocation);

			// 액터 스폰
			AGolemMeteor* SpawnedActor = GetWorld()->SpawnActor<AGolemMeteor>(ActorToSpawn, SpawnTransform, SpawnParams);
			if (CustomTimeDilation == 2.f)
			{
				SpawnedActor->CustomTimeDilation = FMath::FRandRange(1.5f, 2.f);
			}
		}
	}
}

void AMeteorSpawnActor::SpawnRandomLocationNoNav()
{
	TSubclassOf<AActor> ActorToSpawn = AGolemMeteor::StaticClass();

	FVector Origin = GetActorLocation(); // 기준 위치
	float Radius = 5000.0f;

	// 랜덤 위치 계산 (공중 포함)
	FVector RandomLocation = Origin + FMath::VRand() * Radius;

	// 중력 방향 체크
	AMotePlayerController* MoteController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (MoteController)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(MoteController->GetCharacter());
		if (PlayerCharacter->GetGravityDirection() != FVector(0.f, 0.f, -1.0f))
		{
			RandomLocation += FVector((PlayerCharacter->GetGravityDirection()).Normalize());
		}
		else
		{
			RandomLocation.Z += 3000.0f;
		}
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FRotator Rotation = FRotator::ZeroRotator;
	FTransform SpawnTransform(Rotation, RandomLocation);

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnTransform, SpawnParams);
}