// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Monster/Golem/GolemAnimInstance.h"
#include "AI/Monster/Golem/Golem.h"
#include "AI/Monster/Golem/GolemMeteor.h"
#include "AIController.h"
#include "../../../Sound/SoundSubsystem.h"

UGolemAnimInstance::UGolemAnimInstance()
{	
	
	// CameraShake Regist
	mCameraShake.SetNum((int32)EMonsterShakeType::MAX);
	{
		static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera(TEXT("/Script/Engine.Blueprint'/Game/AI/Monster/Boss/Blueprint/BP_GolemDefaultShake.BP_GolemDefaultShake_C'"));
		if (ShakeCamera.Succeeded())
			mCameraShake[(int32)EMonsterShakeType::Impact] = ShakeCamera.Class;
	}
	{
		static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera2(TEXT("/Script/Engine.Blueprint'/Game/AI/Monster/Boss/Blueprint/BP_GolemStartingShake.BP_GolemStartingShake_C'"));
		if (ShakeCamera2.Succeeded())
			mCameraShake[(int32)EMonsterShakeType::Starting] = ShakeCamera2.Class;
	}

	{
		static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCamera3(TEXT("/Script/Engine.Blueprint'/Game/AI/Monster/Boss/Blueprint/BP_GolemDefaultShake.BP_GolemDefaultShake_C'"));
		if (ShakeCamera3.Succeeded())
			mCameraShake[(int32)EMonsterShakeType::StrongAttack] = ShakeCamera3.Class;
	}
}

void UGolemAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	mOwner = Cast<AGolem>(GetOwningActor());

	if (mOwner)
		mMovement = mOwner->GetCharacterMovement();

}

void UGolemAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UGolemAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (mOwner)
		mLookDir = mOwner->GetLookTargetDir();
}


void UGolemAnimInstance::PlayStaggerMontage(bool Play)
{
	if (!IsValid(mStaggerMontage))
		return;

	if (!Montage_IsPlaying(mStaggerMontage) && Play)
	{
		Montage_SetPosition(mStaggerMontage, 0.f);
		Montage_Play(mStaggerMontage);
	}

	if (Montage_IsPlaying(mStaggerMontage) && !Play)
	{
		Montage_SetPosition(mStaggerMontage, 0.f);
		Montage_Stop(0.2f, mStaggerMontage);
	}
}

void UGolemAnimInstance::AnimNotify_Shake()
{
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(PlayerController))
	{
		PlayerController->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EMonsterShakeType::Impact]);
	}
}

void UGolemAnimInstance::AnimNotify_Starting()
{
	APlayerController* PlayerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (IsValid(PlayerController))
		PlayerController->PlayerCameraManager->StartCameraShake(mCameraShake[(int32)EMonsterShakeType::Starting]);

}

void UGolemAnimInstance::AnimNotify_StartingEnd()
{
	if (mOwner)
	{
		mIsStarting = false;

		AAIController* Control = mOwner->GetController<AAIController>();
		if (Control)
		{
			Control->GetBlackboardComponent()->SetValueAsBool(CBossGolemKey::mStarting, mIsStarting);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Not Exist AI Control"));
		}

		mOwner->SetGolemAnim(EGolemState::Idle);
		mOwner->SelectedTarget(true);

	}
}

void UGolemAnimInstance::AnimNotify_StateStart()
{
}

void UGolemAnimInstance::AnimNotify_StateEnd()
{
	if (mOwner)
	{
		Montage_Stop(0.2f,mStaggerMontage);
		mOwner->SetStagger(mOwner->mStaggerMax);
		mOwner->SetGolemAnim(EGolemState::Idle);
		AAIController* Control = mOwner->GetController<AAIController>();
		if (Control)
		{
			Control->GetBlackboardComponent()->SetValueAsBool(CBossGolemKey::mStagger, false);
		}
	}
}

void UGolemAnimInstance::AnimNotify_AttackEnd()
{
	AAIController* Control = mOwner->GetController<AAIController>();

	if (Control)
	{
		Control->GetBlackboardComponent()->SetValueAsBool(CBossGolemKey::mCanAttack, false);
	}
}

void UGolemAnimInstance::AnimNotify_SpawnMeteor()
{
	mCanSpawnMeteor = true;
}

void UGolemAnimInstance::AnimNotify_GolemDeathEnd()
{
	mIsDead = true;
	mIsDeathEnd = true;
	mOwner->OffFX();
}

void UGolemAnimInstance::AnimNotify_GolemDeathStart()
{
	if (mOwner)
	{
		mIsDeadStart = true;

		mBGM = mOwner->GetGameInstance()->GetSubsystem<USoundSubsystem>();
		if (mBGM)
		{
			mBGM->StopBGM(3.f);
		}

		mDeathBGM = mOwner->GetGameInstance()->GetSubsystem<USoundSubsystem>();
		if (mBGM)
		{
			mBGM->PlayBGM(TEXT("BossDeath"), 1.5f);
		}
	}
}

void UGolemAnimInstance::AnimNotify_OnBGM()
{
	if (mOwner)
	{
		UGameInstance* GameInstance = mOwner->GetGameInstance();
		if(GameInstance)
			mBGM = GameInstance->GetSubsystem<USoundSubsystem>();
		if (mBGM)
		{
			mBGM->PlayBGM(TEXT("BossBGM"), 0.4f);
		}
	}

}
