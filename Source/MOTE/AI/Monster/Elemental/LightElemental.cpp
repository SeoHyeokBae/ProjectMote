// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Elemental/LightElemental.h"
#include "AI/MonsterAnimTemplate.h"
#include "GameMode/SubGameMode.h"
#include "Interaction/Portal.h"
#include "AIController.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

ALightElemental::ALightElemental()
{

	Create();
	Load();
	Init();
	mMonsterInfoKey = TEXT("LightElemental");
}

void ALightElemental::SetAIType(EAIType Type)
{
	Super::SetAIType(Type);

	EMonsterAnimState Anim = EMonsterAnimState::Idle;

	switch (mAIType)
	{
	case EAIType::Idle:
		Anim = EMonsterAnimState::Idle;
		break;
	case EAIType::Patrol:
		Anim = EMonsterAnimState::Walk;
		break;
	case EAIType::Trace:
		Anim = EMonsterAnimState::Run;
		break;
	case EAIType::Attack:
		Anim = EMonsterAnimState::Attack;
		break;
	case EAIType::Death:
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		if(GetWorld()->GetAuthGameMode<ASubGameMode>()) CreatePortal();
		Anim = EMonsterAnimState::Death;
		break;
	}

	mLightElementalAnim->ChangeAnim(Anim);
}

void ALightElemental::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTargetDist();


}

void ALightElemental::BeginPlay()
{
	Super::BeginPlay();
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore); // 시야 감지 트레이스 무시

	mLightElementalAnim = Cast<UMonsterAnimTemplate>(GetMesh()->GetAnimInstance());

	mLightElementalAnim->SetAnimData(TEXT("LightElemental"));

	SetTeamID(10);
	GetGenericTeamId();

	RandomPatrolPoint();
}

float ALightElemental::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	mLightElementalAnim->PlayMontage(TEXT("Hit"));

	return DamageAmount;
}

void ALightElemental::HitInteraction()
{
}

void ALightElemental::HitStopStart()
{
}

void ALightElemental::HitStopEnd()
{
}

void ALightElemental::Attack()
{
	FVector	Start = GetActorLocation() + GetActorForwardVector() * 200.f;
	FVector	End = Start;

	FCollisionQueryParams	param;
	param.AddIgnoredActor(this);

	TArray<FHitResult>	result;
	TSet<AActor*> DamagedActors;
	bool Collision = GetWorld()->SweepMultiByChannel(result, Start, End,
		FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel8,
		FCollisionShape::MakeSphere(150.f), param);

	if (Collision)
	{
		// 배열 개수만큼 반복하며 하나씩 Hit에 꺼내온다.
		for (auto& Hit : result)
		{
			AActor* player = Hit.GetActor();
			if (player && !DamagedActors.Contains(player))
			{
				DamagedActors.Add(player);
				FDamageEvent	DmgEvent;

				Hit.GetActor()->TakeDamage(5.f, DmgEvent, GetController(), this);

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), mHitEffect, Hit.Location, FRotator::ZeroRotator, FVector(1.5f, 1.5f, 1.5f));
			}
		}
	}
}


void ALightElemental::UpdateTargetDist()
{
	if (mAIType == EAIType::Death)
		return;

	AAIController* MonsterController = Cast<AAIController>(GetController());

	if (MonsterController)
	{
		AActor* Target = Cast<AActor>(MonsterController->GetBlackboardComponent()->GetValueAsObject(CMonsterDefaultKey::mTarget));

		if (IsValid(Target))
		{
			FVector		TargetLoc = Target->GetActorLocation();
			FVector		SourceLoc = GetActorLocation();
			
			float Dis = FVector::Distance(TargetLoc ,SourceLoc);
			MonsterController->GetBlackboardComponent()->SetValueAsFloat(TEXT("Distance"), Dis);
		}
	}
}

void ALightElemental::CreatePortal()
{
	ASubGameMode* GameMode = GetWorld()->GetAuthGameMode<ASubGameMode>();

	if (GameMode)
		GameMode->AddScore(1);

	if (GameMode->GetScore() > 4)
	{
		FActorSpawnParameters SpawnParams;
		
		FRotator Rotation = FRotator(0.0f, -90.0f, 0.0f);
		FVector Translation = FVector(400.f, 5000.f, 400.0f);
		FVector Scale = FVector(1.0f, 1.0f, 1.0f);

		AAIController* MonsterController = Cast<AAIController>(GetController());
		if (MonsterController)
		{
			ACharacter* Target = Cast<ACharacter>(MonsterController->GetBlackboardComponent()->GetValueAsObject(CMonsterDefaultKey::mTarget));
			if (IsValid(Target))
			{
				APlayerController* PlayerController = Cast<APlayerController>(Target->GetController());
				if (PlayerController)
				{
					APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
					if (CameraManager)
					{
						float CameraYaw = CameraManager->GetCameraRotation().Yaw;
						FRotator CamYawRot = FRotator(0.f, CameraYaw, 0.f);
						FVector CamYawDir = CamYawRot.Vector();
						Translation = Target->GetActorLocation() + CamYawDir * CREATE_PORTAL_DIST + FVector(0.f, 0.f, 400.0f);
					}
				}
			}
		}

		FTransform SpawnTransform(Rotation, Translation, Scale);

		APortal* portal = GetWorld()->SpawnActor<APortal>(APortal::StaticClass(), SpawnTransform, SpawnParams);
	}

}

void ALightElemental::Create()
{
	mShellMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShellMesh"));
}

void ALightElemental::Load()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/LightElemental/Meshes/SK_LightElementalCore.SK_LightElementalCore'"));

	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMeshAsset(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		ShellMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/LightElemental/Meshes/SK_LightElementalShell.SK_LightElementalShell'"));

	if (ShellMeshAsset.Succeeded())
		mShellMesh->SetSkeletalMeshAsset(ShellMeshAsset.Object);

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		ElementalAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/AI/Monster/Basic/Elemental/ABP_LightElemental.ABP_LightElemental_C'"));

	if (ElementalAnim.Succeeded())
		GetMesh()->SetAnimInstanceClass(ElementalAnim.Class);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		NiagaraSlashAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/FX/SlashHitAndStabHit/Particles/Niagara/NS_ky_stabHit02.NS_ky_stabHit02'"));

	if (NiagaraSlashAsset.Succeeded())
		mHitEffect = NiagaraSlashAsset.Object;
}

void ALightElemental::Init()
{
	mShellMesh->SetupAttachment(GetMesh());
	mShellMesh->SetLeaderPoseComponent(GetMesh());

	GetCapsuleComponent()->SetCapsuleHalfHeight(92.f);
	GetCapsuleComponent()->SetCapsuleRadius(34.f);
	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -110.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Monster"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Block);

	GetCapsuleComponent()->SetHiddenInGame(true);

	SetActorScale3D(FVector(1.8, 1.8, 1.8));

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mShellMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
