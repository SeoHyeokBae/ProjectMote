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
	// ������ ���� Ŭ����
	TSubclassOf<AActor> ActorToSpawn = AGolemMeteor::StaticClass();

	// ������ġ ������ ���� ����޽�
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (NavSystem)
	{
		FVector BaseLocation = GetActorLocation(); // �Ǵ� �÷��̾� ��ġ, Ư�� ����Ʈ ��

		// ���� ��ġ�� ã�� ���� �ݰ�
		float Radius = 6000.0f;

		// �Լ��� ��ȯ�ϴ� ���� ��ġ��
		// FNavLocation.Location 
		FNavLocation RandomLocation;

		// �׺���̼� �����Ͱ� �ִ��� Ȯ���մϴ�.
		if (NavSystem->GetRandomReachablePointInRadius(BaseLocation, Radius, RandomLocation))
		{
			// ���� �Ķ���� ����
			FActorSpawnParameters SpawnParams;

			// �߷� ���� üũ
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
			// ���� ��ġ�� ��ø�Ǿ� �浹�� ���ϵ��� ��ġ�� �����Ͽ� ����
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// ��ġ�� ȸ�� �� ����
			FRotator Rotation = FRotator::ZeroRotator;
			FTransform SpawnTransform(Rotation, RandomLocation);

			// ���� ����
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

	FVector Origin = GetActorLocation(); // ���� ��ġ
	float Radius = 5000.0f;

	// ���� ��ġ ��� (���� ����)
	FVector RandomLocation = Origin + FMath::VRand() * Radius;

	// �߷� ���� üũ
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