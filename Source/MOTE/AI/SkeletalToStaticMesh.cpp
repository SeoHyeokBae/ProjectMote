

#include "AI/SkeletalToStaticMesh.h"
#include "Components/PoseableMeshComponent.h"

ASkeletalToStaticMesh::ASkeletalToStaticMesh()
{
	PrimaryActorTick.bCanEverTick = false;

	mPoseableComponent = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMeshComponent"));
	mPoseableComponent->SetupAttachment(RootComponent);
}

void ASkeletalToStaticMesh::BeginPlay()
{
	Super::BeginPlay();
}

void ASkeletalToStaticMesh::Init(USkeletalMeshComponent* SkeletalMeshComponent)
{
	if (SkeletalMeshComponent)
		return;
		
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mPoseableComponent->SetSkeletalMesh(SkeletalMeshComponent->SkeletalMesh);
	mPoseableComponent->CopyPoseFromSkeletalComponent(SkeletalMeshComponent);
}

