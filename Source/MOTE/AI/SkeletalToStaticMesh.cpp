// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SkeletalToStaticMesh.h"
#include "Components/PoseableMeshComponent.h"

// Sets default values
ASkeletalToStaticMesh::ASkeletalToStaticMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mPoseableComponent = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMeshComponent"));
	mPoseableComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASkeletalToStaticMesh::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASkeletalToStaticMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkeletalToStaticMesh::Init(USkeletalMeshComponent* SkeletalMeshComponent)
{
	if (SkeletalMeshComponent)
		return;
		
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mPoseableComponent->SetSkeletalMesh(SkeletalMeshComponent->SkeletalMesh);
	mPoseableComponent->CopyPoseFromSkeletalComponent(SkeletalMeshComponent);
}

