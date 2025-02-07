// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerTitlePawn.h"
#include "Player/BaseWeapon.h"

// Sets default values
APlayerTitlePawn::APlayerTitlePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	mArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DashArrow"));
	mArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("CharacterLight"));
	mCloak_R = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Cloak_Right"));
	mCloak_L = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Cloak_Left"));
	mCloak_R->AttachToComponent(mMesh, FAttachmentTransformRules::KeepRelativeTransform, FName("Cloak"));
	mCloak_L->AttachToComponent(mMesh, FAttachmentTransformRules::KeepRelativeTransform, FName("Cloak"));

	// Load
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>
			MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Player/HollowKnight/Mesh/HollowKnight.HollowKnight'"));

		if (MeshAsset.Succeeded())
			mMesh->SetSkeletalMeshAsset(MeshAsset.Object);
	}

	{
		static ConstructorHelpers::FClassFinder<UAnimInstance>
			PlayerAnimClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Player/ABP_PlayerTitle.ABP_PlayerTitle_C'"));

		if (PlayerAnimClass.Succeeded())
			mMesh->SetAnimInstanceClass(PlayerAnimClass.Class);
	}

	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>
			Cloak_R(TEXT("/Script/Engine.SkeletalMesh'/Game/Player/HollowKnight/Equip/LeftCoak.LeftCoak'"));

		if (Cloak_R.Succeeded())
			mCloak_R->SetSkeletalMeshAsset(Cloak_R.Object);

		static ConstructorHelpers::FObjectFinder<USkeletalMesh>
			Cloak_L(TEXT("/Script/Engine.SkeletalMesh'/Game/Player/HollowKnight/Equip/RightCoak.RightCoak'"));

		if (Cloak_L.Succeeded())
			mCloak_L->SetSkeletalMeshAsset(Cloak_L.Object);
	}

	{
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
			NiagaraFloatAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Player/FX/NS_PlayerFloatFX.NS_PlayerFloatFX'"));

		if (NiagaraFloatAsset.Succeeded())
			mEffect = NiagaraFloatAsset.Object;
	}

	mMesh->SetRelativeLocation(FVector(0.0, 0.0, -93.0));
	mMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

	SetRootComponent(mMesh);
	mCamera->SetupAttachment(mArm);
	mArm->SetupAttachment(RootComponent);
	mLight->SetupAttachment(RootComponent);
	mArrow->SetupAttachment(RootComponent);
	mMovement->SetUpdatedComponent(RootComponent);
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mArrow->SetRelativeLocation(FVector(0.0, 0.0, -15.0));

	// Camera Rotate Set
	bUseControllerRotationYaw = false;
	mArm->bInheritPitch = false;
	mArm->bInheritYaw = false;
	mArm->bInheritRoll = false;
	mArm->bEnableCameraLag = true;

	mArm->SetRelativeLocation(FVector(0.0, 0.0, 170.f));
	mArm->SetRelativeRotation(FRotator(-10.0, -53.0, 0.0));
	mArm->TargetArmLength = 800.f;

	mLight->SetRelativeLocation(FVector(0.0, 0.0, 3000.0));
	mLight->SetRelativeRotation(FRotator(-90.0, 0.0, 0.0));
	mLight->SetMobility(EComponentMobility::Movable);
	mLight->SetIntensityUnits(ELightUnits::Lumens);
	mLight->SetIntensity(1500.0);
	mLight->SetAttenuationRadius(16384.0);
	mLight->SetInnerConeAngle(80.0);
	mLight->SetOuterConeAngle(80.0);
	mLight->SetSourceRadius(1000.0);
	mLight->SetSoftSourceRadius(1000.0);
}

// Called when the game starts or when spawned
void APlayerTitlePawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerTitlePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerTitlePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerTitlePawn::EffectOn()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(mEffect, mArrow, NAME_None, mArrow->GetRelativeLocation()
		, FRotator(0.f, 0.f, 0.f), EAttachLocation::KeepRelativeOffset, true);
}

