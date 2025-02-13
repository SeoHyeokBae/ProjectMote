// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGhostTrail.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values
APlayerGhostTrail::APlayerGhostTrail()
{
	PrimaryActorTick.bCanEverTick = true;

	mPoseableMeshComponent = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("GhostTrail"));
	RootComponent = mPoseableMeshComponent;
	mPoseableMeshComponent->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -97.f), FRotator(0.f, 270.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance>
		Material(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Player/FX/MTI_GhostTrail.MTI_GhostTrail'"));

	if (Material.Succeeded())
		mMtrlInstance = Material.Object;
}

void APlayerGhostTrail::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerGhostTrail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (mIsSpawned)
	{
		mFadeCountDown -= DeltaTime;
		for (int i = 0; i < mMaterials.Num(); ++i)
		{
			mMaterials[i]->SetScalarParameterValue("Opacity", mFadeCountDown / mFadeOutTime);
		}
		if (mFadeCountDown < 0)
		{
			Destroy();
		}
	}
}

void APlayerGhostTrail::Init(USkeletalMeshComponent* SkeletalMeshComponent)
{
	mPoseableMeshComponent->SetSkeletalMesh(SkeletalMeshComponent->SkeletalMesh);
	mPoseableMeshComponent->CopyPoseFromSkeletalComponent(SkeletalMeshComponent);
	TArray<UMaterialInterface*> Mats = mPoseableMeshComponent->GetMaterials();

	for (int i = 0; i < Mats.Num(); ++i)
	{
		mMaterials.Add(UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), mMtrlInstance));
		mPoseableMeshComponent->SetMaterial(i, mMaterials[i]);
	}

	mFadeCountDown = mFadeOutTime;
	mIsSpawned = true;
}

