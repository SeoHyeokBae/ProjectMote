// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/LockOnLaser.h"
#include "AI/Monster/Golem/Golem.h"
#include "AI/Monster/Golem/GolemAnimInstance.h"
#include "../../../Player/PlayerCharacter.h"

ALockOnLaser::ALockOnLaser()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	mMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Player/FX/ETC/LaserTargetCylinder.LaserTargetCylinder'"));
	if (MeshAsset.Succeeded())
		mMesh->SetStaticMesh(MeshAsset.Object);

	mMtrlInst = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), nullptr
		, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Player/FX/ETC/MTI_LaserTarget.MTI_LaserTarget'")));

	mMtrlInstDynamic = UMaterialInstanceDynamic::Create(mMtrlInst, this);

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Script/Engine.Material'/Game/Player/FX/ETC/MT_LaserTarget.MT_LaserTarget'"));
	if (MaterialAsset.Succeeded())
		mMesh->SetMaterial(0, MaterialAsset.Object); 
	
	RootComponent = mRoot;
	mMesh->SetupAttachment(RootComponent);
	mArrow->SetupAttachment(RootComponent);

	mMesh->SetRelativeScale3D(FVector(1.0, 0.3, 0.3));
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mArrow->SetRelativeRotation(FRotator(0, 180, 0));
}

void ALockOnLaser::BeginPlay()
{
	Super::BeginPlay();

	mMesh->SetMaterial(0, mMtrlInstDynamic);
}

void ALockOnLaser::Tick(float Deltatime)
{
	Super::Tick(Deltatime);

	UpdateLength(Deltatime);
}

void ALockOnLaser::UpdateLength(float Deltatime)
{
	if (mStartTime < 0.4f)
	{
		mStartTime += Deltatime;
		return;
	}

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	FVector Start = mArrow->GetComponentLocation();
	FVector End = Start + mArrow->GetForwardVector() * 15000.f;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel12, CollisionParams);
	float Length = 0.f;
	
	mTotalAcc += Deltatime;
	if (bHit)
	{
		Length = (HitResult.TraceStart - HitResult.Location).Length();

		APlayerCharacter* Player = Cast<APlayerCharacter>(HitResult.GetActor());
		if (Player)
		{
			mLockOnAcc += Deltatime;

			if ((mLockOnAcc >= mLockOnTime) && !mIsFinish)
			{
				mIsFinish = true;
				mIsLockOn = true;
				mLockOnAcc = mLockOnTime;
				AGolem* Golem = Cast<AGolem>(GetOwner());
				if (Golem)
				{
					UGolemAnimInstance* Anim = Cast<UGolemAnimInstance>(Golem->GetMesh()->GetAnimInstance());
					if (Anim)
					{
						Golem->SetGolemAnim(EGolemState::Skill_Lazer);
						Destroy();
					}
				}
			}
		}
	}
	else
	{
		if (mTotalAcc >= mMissTargetTime && !mIsFinish)
		{
			mIsFinish = true;
			mIsLockOn = false;
			AGolem* Golem = Cast<AGolem>(GetOwner());
			if (Golem)
			{
				UGolemAnimInstance* Anim = Cast<UGolemAnimInstance>(Golem->GetMesh()->GetAnimInstance());
				if (Anim)
				{
					Golem->SetGolemAnim(EGolemState::Skill_Lazer);
					Destroy();
				}
			}
		}
	}

	if (Length == 0.f)
		Length = (HitResult.TraceStart - HitResult.TraceEnd).Length();

	float Scale = Length / CylinerSize;
	mMesh->SetRelativeScale3D(FVector(Scale, 0.9f, 0.9f));

	float Red = DefaultColorParam * FMath::Clamp((mLockOnAcc * 2.f / (mLockOnTime * 0.9)), 0.0f, 2.0f);
	float Blue = DefaultColorParam * FMath::Clamp(1.0f - (mLockOnAcc / mLockOnTime), 0.0f, 1.0f);

	mMtrlInstDynamic->SetVectorParameterValue(TEXT("Color"), FVector4(Red, 0.1, Blue, 1.0));
}