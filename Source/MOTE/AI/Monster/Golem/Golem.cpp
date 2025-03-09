// Fill out your copyright notice in the Description page of Project Settings.


#include "Golem.h"
#include "AI/Monster/Golem/GolemAnimInstance.h"
#include "AI/Monster/TargetAimSystemComponent.h"
#include "Controller/GolemController.h"


AGolem::AGolem()
{
	PrimaryActorTick.bCanEverTick = true;

	Create();
	Load();
	Init();
	
	mMonsterInfoKey = TEXT("Golem");
}

void AGolem::SetGolemAnim(EGolemState Type)
{
	mState = Type;
	EGolemAnim Anim = EGolemAnim::Idle;

	switch (mState)
	{
	case EGolemState::Starting:
		Anim = EGolemAnim::Starting;
		break;
	case EGolemState::Idle:
		Anim = EGolemAnim::Idle;
		break;
	case EGolemState::Attack:
		Anim = EGolemAnim::Attack;
		break;
	case EGolemState::Skill_Crarge:
		Anim = EGolemAnim::Skill_Crarge;
		break;
	case EGolemState::Skill_Meteor:
		Anim = EGolemAnim::Skill_Meteor;
		break;
	case EGolemState::Skill_Lazer:
		Anim = EGolemAnim::Skill_Lazer;
		break;
	case EGolemState::Skill_Lazer_Charge:
		Anim = EGolemAnim::Skill_Lazer_Charge;
		break;
	case EGolemState::Skill_Crash:
		Anim = EGolemAnim::Skill_Crash;
		break;
	case EGolemState::Stagger:
	{
		mGolemAnim->PlayStaggerMontage();
		break;
	}
	case EGolemState::Death:
		Anim = EGolemAnim::Death;
		mGolemAnim->PlayStaggerMontage(false);
		break;
	}

	if (IsValid(mGolemAnim))
		mGolemAnim->ChangeAnim(Anim);
}

void AGolem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	UpdateTargetDir();
}

void AGolem::BeginPlay()
{
    Super::BeginPlay();

	mTargetAimSystem->GetLockOnWidget()->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("spine_03"));

	if (IsValid(mVFXMesh))
	{
		mFlameFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			mFlameFXSystem, mVFXMesh, NAME_None, mVFXMesh->GetRelativeLocation()
			, FRotator(0.f, 0.f, 0.f), EAttachLocation::KeepRelativeOffset, true); 
		
		mSmokeFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			mSmokeFXSystem, mVFXMesh, FName("pelvis"), FVector(-15.0, 0.0, 0.0)
			, FRotator(-90.0f, -0.5f, 0.5f), EAttachLocation::KeepRelativeOffset, true);
	}

	GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &AGolem::MeshBeginOverlap);
	GetMesh()->OnComponentHit.AddDynamic(this, &AGolem::MeshOnHit);

	mGolemAnim = Cast<UGolemAnimInstance>(GetMesh()->GetAnimInstance());

	AddDeathDelegate<ThisClass>(this, &AGolem::DeathDelegate);
	
	mState = EGolemState::Starting; // 테스트 state
	SetGolemAnim(mState);

	// 블랙보드 초기 공격 상태
	AAIController* Control = GetController<AAIController>();
	if (Control)
	{
		Control->GetBlackboardComponent()->SetValueAsBool(CBossGolemKey::mCanAttack, true);
		Control->GetBlackboardComponent()->SetValueAsBool(CBossGolemKey::mStarting, true);
	}
}

float AGolem::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (mStagger <= 0.f && mState != EGolemState::Death)
	{
		mState = EGolemState::Stagger;
		SetGolemAnim(EGolemState::Stagger);
		AAIController* Control =GetController<AAIController>();
		if (Control)
		{
			Control->GetBlackboardComponent()->SetValueAsBool(CBossGolemKey::mStagger, true);
		}
	}
    return DamageAmount;
}

void AGolem::Attack()
{
}

bool AGolem::GetCanSpawnMeteor()
{
	return mGolemAnim->GetCanSpawnMeteor();
}

void AGolem::SetSpawnMeteor()
{
	mGolemAnim->SetSpawnMeteor();
}

void AGolem::UpdateTargetDir()
{
	if (mState == EGolemState::Skill_Crarge || mState == EGolemState::Stagger)
		return;

	AAIController* MonsterController = Cast<AAIController>(GetController());
	if (MonsterController)
	{
		AActor* Target = Cast<AActor>(MonsterController->GetBlackboardComponent()->GetValueAsObject(CMonsterDefaultKey::mTarget));
		
		if (IsValid(Target))
		{
			FVector		TargetLoc = Target->GetActorLocation();
			FVector		SourceLoc = GetActorLocation();
			//FVector		SourceLoc = GetMesh()->GetSocketLocation("head");

			FVector		Direction = TargetLoc - SourceLoc;
			FRotator	Rot = Direction.Rotation();

			// 타켓 방향으로 컨트롤러 회전
			// 컨트롤러는 지속적으로 캐릭터 방향으로 회전을 맞춰줌
			MonsterController->SetControlRotation(Rot);

			FVector MeshVector = GetRootComponent()->GetForwardVector();

			FRotator PrevRot = mLookTargetDir;
			FRotator TargetRot = Rot - MeshVector.Rotation();

			const FRotator RInterpToRotator = FMath::RInterpTo(PrevRot, TargetRot,
				GetWorld()->GetDeltaSeconds(), 3.0f);

			mLookTargetDir = RInterpToRotator;

			float Dis = Direction.Length();
			MonsterController->GetBlackboardComponent()->SetValueAsFloat(TEXT("Distance"), Dis);
		}
		else
		{
			const FRotator RInterpToRotator = FMath::RInterpTo(mLookTargetDir, FRotator::ZeroRotator,
				GetWorld()->GetDeltaSeconds(), 5.0f);

			mLookTargetDir = RInterpToRotator;
		}
	}
}


void AGolem::HitInteraction()
{
}

void AGolem::HitStopStart()
{
	FTimerHandle TimerHandle;

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2f);

	//if (mState == EGolemState::Attack || mState == EGolemState::Skill_Crash || mState == EGolemState::Skill_Lazer || mState == EGolemState::Skill_Crarge)
	//{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGolem::HitStopEnd, 0.055f, false);

		APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		if (IsValid(PlayerController))
		{
			PlayerController->PlayerCameraManager->StartCameraShake(mGolemAnim->GetGolemShakeBase(int32(EMonsterShakeType::Impact)));
		}
	//}
}

void AGolem::HitStopEnd()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
}

void AGolem::MeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AGolem::MeshOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void AGolem::DeathDelegate()
{
	if (mAIType == EAIType::Death)
	{
		SetGolemAnim(EGolemState::Death);
	}
}

void AGolem::Create()
{
	mArmorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorMesh"));
	mVFXMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VFXMesh"));

	AIControllerClass = AGolemController::StaticClass();
}

void AGolem::Load()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/AI/Monster/Boss/HellGolemWarrior/Meshes/SK_HellGolemWarriorStones.SK_HellGolemWarriorStones'"));

	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMeshAsset(MeshAsset.Object);

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		GolemAnimClass(TEXT("/Script/Engine.AnimBlueprint'/Game/AI/Monster/Boss/ABP_Golem.ABP_Golem_C'"));

	if (GolemAnimClass.Succeeded())
		GetMesh()->SetAnimInstanceClass(GolemAnimClass.Class);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		ArmorMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/AI/Monster/Boss/HellGolemWarrior/Meshes/SK_HellGolemWarriorArmor.SK_HellGolemWarriorArmor'"));

	if (ArmorMeshAsset.Succeeded())
		mArmorMesh->SetSkeletalMeshAsset(ArmorMeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		VFXMeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/AI/Monster/Boss/HellGolemWarrior/Meshes/SK_HellGolemWarriorForVFX.SK_HellGolemWarriorForVFX'"));

	if (VFXMeshAsset.Succeeded())
		mVFXMesh->SetSkeletalMeshAsset(VFXMeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		NiagaraFlameAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/AI/Monster/Boss/HellGolemWarrior/VFX/FXS_HellGolemWarriorFlame.FXS_HellGolemWarriorFlame'"));

	if (NiagaraFlameAsset.Succeeded())
		mFlameFXSystem = NiagaraFlameAsset.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		NiagaraSmokeAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/AI/Monster/Boss/HellGolemWarrior/VFX/FXS_HellGolemWarriorSmoke.FXS_HellGolemWarriorSmoke'"));

	if (NiagaraSmokeAsset.Succeeded())
		mSmokeFXSystem = NiagaraSmokeAsset.Object;

}

void AGolem::Init()
{
	mArmorMesh->SetupAttachment(GetMesh());
	mArmorMesh->SetLeaderPoseComponent(GetMesh());
	mVFXMesh->SetupAttachment(GetMesh());
	mVFXMesh->SetLeaderPoseComponent(GetMesh());

	mTargetAimSystem->SetupAttachment(GetMesh(), FName("spine_03"));

	GetCapsuleComponent()->SetCapsuleHalfHeight(1250.f);
	GetCapsuleComponent()->SetCapsuleRadius(700.f);
	//GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetHiddenInGame(true);
	//GetCapsuleComponent()->SetMassOverrideInKg(NAME_None, 5000.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("GolemCapsule"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Block);

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -2740.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	GetMesh()->SetRelativeScale3D(FVector(30.0, 30.0, 20.0));
	FWalkableSlopeOverride SlopeOverride;
	SlopeOverride.WalkableSlopeAngle = 45.0f;
	GetMesh()->SetWalkableSlopeOverride(SlopeOverride);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	//mTargetAimSystem->SetupAttachment(RootComponent);

	mAIType = EAIType::Boss;
}
