// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BaseWeapon.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mBasePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BaseArrow"));
	mTipPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("TipArrow"));

	SetRootComponent(mRoot);
	mMesh->SetupAttachment(RootComponent);
	mBasePoint->SetupAttachment(RootComponent);
	mTipPoint->SetupAttachment(RootComponent);

	mBasePoint->SetRelativeLocation(FVector(75.0, 0.0, 0.0));
	mBasePoint->SetRelativeRotation(FRotator(90.0, 90.0, 0.0));
	mTipPoint->SetRelativeLocation(FVector(-225.0, 0.0, 25.0));
	mTipPoint->SetRelativeRotation(FRotator(90.0, 90.0, 0.0));

	//static ConstructorHelpers::FObjectFinder<USkeletalMesh>
	//	MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/KoreanTraditionalMartialArts/Meshs/Weapons/Meshs/SKM_Hwando.SKM_Hwando'"));

	//if (MeshAsset.Succeeded())
	//	mMesh->SetSkeletalMeshAsset(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Weapons/EnergySword/CyberpunkKatana/source/CyberSword/CyberSword.CyberSword'"));

	if (MeshAsset.Succeeded())
		mMesh->SetStaticMesh(MeshAsset.Object);
	SetActorScale3D(FVector(0.4, 0.4, 0.4));

	mMesh->bReceivesDecals = false;
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

